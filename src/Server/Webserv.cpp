#include "Webserv.hpp"

#include "Http/Request.hpp"
#include "Http/Response.hpp"

#include <arpa/inet.h>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace Hafserv;

Webserv &Webserv::getInstance()
{
	static Webserv webserv;
	return webserv;
}

Webserv::Webserv()
{
	initWebserv();
	statusCodeMap[200] = "OK";
	statusCodeMap[201] = "Created";
	statusCodeMap[202] = "Accepted";
	statusCodeMap[204] = "No Content";
	statusCodeMap[400] = "Bad Request";
	statusCodeMap[403] = "Forbidden";
	statusCodeMap[404] = "Not Found";
	statusCodeMap[405] = "Not Allowed";
	statusCodeMap[411] = "Length Required";
	statusCodeMap[413] = "Content Too Large";
	statusCodeMap[431] = "Request Header Fields Too Large";
	statusCodeMap[500] = "Internal Server Error";
	statusCodeMap[501] = "Not Implemented";
	statusCodeMap[505] = "HTTP Version Not Supported";
}

Webserv::~Webserv()
{
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
		delete *it;
}

void Webserv::addServer(Server *server)
{
	servers.push_back(server);

	for (std::set<unsigned short>::const_iterator it = server->getPorts().begin(); it != server->getPorts().end(); it++)
		openPort(*it);
}

void Webserv::openPort(unsigned short port)
{
	if (portToServSock.find(port) == portToServSock.end())
	{
		struct sockaddr_in serv_adr;
		struct kevent event;
		int socketFd;

		socketFd = socket(PF_INET, SOCK_STREAM, 0);
		memset(&serv_adr, 0, sizeof(serv_adr));
		serv_adr.sin_family = AF_INET;
		serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_adr.sin_port = htons(port);

		if (bind(socketFd, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		{
			std::cerr << "bind() error" << std::endl;
			std::exit(1);
		}
		if (listen(socketFd, 1000) == -1)
		{
			std::cerr << "listen() error" << std::endl;
			std::exit(1);
		}

		int ret;
		EV_SET(&event, socketFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		ret = kevent(kq, &event, 1, NULL, 0, NULL);

		portToServSock[port] = socketFd;
		servSockToPort[socketFd] = port;
	}
}

void Webserv::initWebserv()
{
	kq = kqueue();
	if (kq == -1)
	{
		std::cerr << "kqueue() error" << std::endl;
		std::exit(1);
	}
}

void Webserv::runWebserv()
{
	struct kevent event_list[MAX_EVENTS];

	struct timespec timeout;
	bzero(&timeout, sizeof(struct timespec));
	while (1)
	{
		int events = kevent(kq, NULL, 0, event_list, MAX_EVENTS, &timeout);
		if (events == -1)
		{
			std::cerr << "kevent() error" << std::endl;
			continue;
		}

		for (int i = 0; i < events; i++)
		{
			int eventFd = event_list[i].ident;
			try
			{
				if (inServSocks(eventFd))
				{
					connectClient(eventFd);
				}
				else if (inClientSocks(eventFd))
				{
					if (event_list[i].flags & EV_EOF)
					{
						disconnectClient(eventFd);
					}
					else if (event_list[i].filter == EVFILT_READ)
					{
						Connection &conn = findConnectionByFd(eventFd);
						if (conn.getRequest().getParseStatus() == End)
							continue;
						conn.readRequest(eventFd);
					}
					else if (event_list[i].filter == EVFILT_WRITE)
					{
						Connection &conn = findConnectionByFd(eventFd);
						if (conn.getResponse().getResponseState() == Response::Ready)
							conn.sendResponse();
						else if (conn.getResponse().getResponseState() == Response::Sending)
							conn.sendResponse();
						if (conn.getResponse().getResponseState() == Response::End)
						{
							if (conn.getStatusCode() || conn.getConnectionClose())
							{
								disconnectClient(eventFd);
							}
							else
							{
								conn.reset();
							}
						}
					}
				}
				else if (inCGISocks(eventFd))
				{
					Connection &conn = findConnectionByFd(cgiFdToConnectionFd.find(eventFd)->second);
					if (event_list[i].filter == EVFILT_WRITE)
					{
						conn.writeToCGI(eventFd);
					}
					else if (event_list[i].filter == EVFILT_READ)
					{
						conn.readFromCGI(eventFd);
					}
				}
			}
			catch (std::exception &e)
			{
				std::cout << "exception in webserv: " << e.what() << std::endl;
				continue;
			}
		}
		checkTimeout();
	}
	closeServSocks();
	close(kq);
}

void Webserv::connectClient(int serv_sock)
{
	int clnt_sock;
	struct sockaddr_in clnt_adr;
	socklen_t adr_sz;
	struct kevent event;

	adr_sz = sizeof(clnt_adr);
	clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
	fcntl(clnt_sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	EV_SET(&event, clnt_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);
	EV_SET(&event, clnt_sock, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);

	int port = servSockToPort.find(serv_sock)->second;
	sockToPort[clnt_sock] = port;

	Connections.insert(std::make_pair(clnt_sock, Connection(clnt_sock, sockToPort.find(clnt_sock)->second)));

	std::cout << "connected client: " << clnt_sock << std::endl;
}

void Webserv::disconnectClient(int socketfd)
{
	struct kevent event;
	EV_SET(&event, socketfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);
	EV_SET(&event, socketfd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);
	close(socketfd);

	Connection &conn = findConnectionByFd(socketfd);
	if (conn.getResponse().getResponseState() == Response::BuildingCGI)
	{
		kill(conn.getCGIPid(), SIGKILL);
		deleteCGIEvent(conn.getReadPipe(), conn.getWritePipe());
	}

	Connections.erase(socketfd);

	std::map<int, unsigned short>::iterator socketToPortIt = sockToPort.find(socketfd);
	if (socketToPortIt != sockToPort.end())
		sockToPort.erase(socketToPortIt);

	std::cout << "closed client: " << socketfd << std::endl;
}

void Webserv::addCGIEvent(int connectionFd, int readPipe, int writePipe)
{
	struct kevent event;

	EV_SET(&event, readPipe, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);
	cgiFdToConnectionFd[readPipe] = connectionFd;
	if (writePipe)
	{
		EV_SET(&event, writePipe, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
		kevent(kq, &event, 1, NULL, 0, NULL);
		cgiFdToConnectionFd[writePipe] = connectionFd;
	}
}

void Webserv::deleteCGIEvent(int readPipe, int writePipe)
{
	struct kevent event;

	EV_SET(&event, readPipe, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(kq, &event, 1, NULL, 0, NULL);
	cgiFdToConnectionFd.erase(readPipe);
	close(readPipe);
	if (writePipe)
	{
		EV_SET(&event, writePipe, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		kevent(kq, &event, 1, NULL, 0, NULL);
		cgiFdToConnectionFd.erase(writePipe);
		close(writePipe);
	}
}

Connection &Webserv::findConnectionByFd(int fd)
{
	ConnectionMap::iterator it = Connections.find(fd);
	if (it == Connections.end())
		throw std::exception();
	return it->second;
}

Server *Webserv::findTargetServer(unsigned short port, const Request &request)
{
	Server *defaultServer = NULL;
	std::string host = request.getHeaders().find("host")->second;

	std::string hostName = host;
	if (host.find(':') != std::string::npos)
		hostName = host.substr(0, host.find(':'));

	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::set<std::string> serverNames = (*it)->getNames();
		std::set<unsigned short> ports = (*it)->getPorts();
		std::set<std::string>::const_iterator findNameIt = serverNames.find(hostName);
		std::set<unsigned short>::iterator findPortIt = ports.find(port);

		if (defaultServer == NULL && findPortIt != ports.end())
			defaultServer = *it;
		if (findNameIt != serverNames.end() && findPortIt != ports.end())
			return *it;
	}
	return defaultServer;
}

std::string Webserv::getStatusMessage(int statusCode)
{
	if (statusCodeMap.find(statusCode) == statusCodeMap.end())
		return util::string::itos(statusCode);
	else
		return util::string::itos(statusCode) + " " + statusCodeMap.find(statusCode)->second;
}

void Webserv::checkTimeout()
{
	time_t now = time(NULL);
	std::vector<int> timeoutSockets;
	for (ConnectionMap::iterator it = Connections.begin(); it != Connections.end(); it++)
	{
		if (it->second.getRequest().getParseStatus() < Body)
		{
			int headerTimeout = 60;
			if (now - it->second.getStartTime() > headerTimeout)
			{
				timeoutSockets.push_back(it->first);
			}
		}
		else if (it->second.getRequest().getParseStatus() == Body)
		{
			const LocationConfig targetConfig = it->second.getTargetLocationConfig();
			int bodyTimeout = 60;
			if (targetConfig.getTimeout().clientHeader > 0)
			{
				bodyTimeout = targetConfig.getTimeout().clientBody;
			}
			if (now - it->second.getStartTime() > bodyTimeout)
			{
				timeoutSockets.push_back(it->first);
			}
		}
		else if (it->second.getResponse().getResponseState() == Response::BuildingCGI)
		{
			int cgiTimeout = 60;
			if (now - it->second.getStartTime() > cgiTimeout)
			{
				timeoutSockets.push_back(it->first);
			}
		}
	}
	for (std::vector<int>::iterator it = timeoutSockets.begin(); it != timeoutSockets.end(); it++)
	{
		disconnectClient(*it);
	}
}

bool Webserv::inServSocks(int fd) { return servSockToPort.find(fd) != servSockToPort.end(); }

bool Webserv::inClientSocks(int fd) { return sockToPort.find(fd) != sockToPort.end(); }

bool Webserv::inCGISocks(int fd) { return cgiFdToConnectionFd.find(fd) != cgiFdToConnectionFd.end(); }

void Webserv::closeServSocks()
{
	for (std::map<int, unsigned short>::iterator it = servSockToPort.begin(); it != servSockToPort.end(); it++)
	{
		close(it->first);
	}
}

const std::map<int, std::string> &Hafserv::Webserv::getStatusCodeMap() const { return statusCodeMap; }
