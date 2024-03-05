#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Http/Connection.hpp"
#include "Http/Request.hpp"
#include "Server.hpp"

#include <map>
#include <vector>

#define BUF_SIZE 512
#define MAX_EVENTS 50
#define CRLF "\r\n"

namespace Hafserv
{

class Connection;

typedef std::map<int, Connection> ConnectionMap;

class Webserv
{
public:
	static Webserv &getInstance();

	void initWebserv();

	void addServer(Server *server);
	void openPort(unsigned short port);

	void runWebserv();

	void connectClient(int serv_sock);
	void disconnectClient(int socketfd);
	void addCGIEvent(int connectionFd, int readPipe, int writePipe);
	void deleteCGIEvent(int readPipe, int writePipe);

	Connection &findConnectionByFd(int fd);
	Server *findTargetServer(unsigned short port, const Request &request);

	std::string getStatusMessage(int statusCode);

	void checkTimeout();

	void closeServSocks();

	const std::map<int, std::string> &getStatusCodeMap() const;

private:
	Webserv();
	~Webserv();

	bool inServSocks(int fd);
	bool inClientSocks(int fd);
	bool inCGISocks(int fd);

	int kq;

	std::map<int, std::string> statusCodeMap;

	std::map<unsigned short, int> portToServSock;
	std::map<int, unsigned short> servSockToPort;
	std::map<int, unsigned short> sockToPort;

	std::map<int, int> cgiFdToConnectionFd;

	std::vector<Server *> servers;

	ConnectionMap Connections;
};

} // namespace Hafserv

#endif
