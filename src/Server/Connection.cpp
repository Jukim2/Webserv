#include "Http/Connection.hpp"

#include "File/Directory.hpp"
#include "File/File.hpp"

#include <cstdio>
#include <fcntl.h>
#include <string>

using namespace Hafserv;

Connection::Connection(int socket, unsigned short port)
	: socket(socket), port(port), statusCode(0), targetServer(NULL), cgiPID(0), readPipe(0), writePipe(0), end(false),
	  connectionClose(false)
{
	startTime = time(NULL);
}

Connection::Connection(const Connection &other)
	: request(other.request), response(other.response), socket(other.socket), port(other.port),
	  statusCode(other.statusCode), startTime(other.startTime), targetServer(other.targetServer),
	  targetLocationConfig(other.targetLocationConfig), targetResource(other.targetResource), cgiPID(0),
	  readPipe(other.readPipe), writePipe(other.writePipe), end(other.end), connectionClose(other.connectionClose)
{
}

Connection &Connection::operator=(Connection &rhs)
{
	if (this != &rhs)
	{
		request = rhs.request;
		response = rhs.response;
		socket = rhs.socket;
		port = rhs.port;
		statusCode = rhs.statusCode;
		startTime = rhs.startTime;
		targetServer = rhs.targetServer;
		targetLocationConfig = rhs.targetLocationConfig;
		targetResource = rhs.targetResource;
		cgiPID = rhs.cgiPID;
		readPipe = rhs.readPipe;
		writePipe = rhs.writePipe;
		end = rhs.end;
		connectionClose = rhs.connectionClose;
	}
	return *this;
}

Connection::~Connection() {}

void Connection::readRequest(int fd)
{
	startTime = time(NULL);
	statusCode = request.readRequest(fd);

	if (statusCode == SYS_ERROR)
	{
		response.setResponseState(Response::End);
		connectionClose = true;
		return;
	}
	if (request.getParseStatus() >= Body)
	{
		if (targetServer == NULL && !statusCode)
		{
			targetServer = Webserv::getInstance().findTargetServer(port, request);
			targetResource = configureTargetResource(request.getRequestTarget().getTargetURI());
		}
		if (targetLocationConfig.getClientMaxBodySize() < getRequest().getBodyLength())
		{
			statusCode = 413;
		}
		if (request.getConnectionClose())
			connectionClose = true;
	}
	if (request.getParseStatus() == End)
	{
		buildResponseFromRequest();
	}
}

std::string Connection::getTargetResource(std::string &requestTarget)
{
	std::string tempTargetResource;
	const std::string &selectedPattern = targetLocationConfig.getPattern();
	const std::string &selectedAlias = targetLocationConfig.getAlias();
	if (!selectedAlias.empty())
	{
		if (selectedPattern.back() == '/')
			tempTargetResource = selectedAlias + requestTarget.substr(selectedPattern.length() - 1);
		else
			tempTargetResource = selectedAlias + requestTarget.substr(selectedPattern.length());
	}
	else
		tempTargetResource = targetLocationConfig.getRoot() + requestTarget;
	if (tempTargetResource.back() == '/')
	{
		if (targetLocationConfig.getAutoIndex())
		{
			return tempTargetResource;
		}
		else
		{
			const std::set<std::string> &indexes = targetLocationConfig.getIndexes();
			std::string defaultTargetResource;
			if (indexes.size() == 0)
				defaultTargetResource = tempTargetResource + "index.html";
			else
				defaultTargetResource = tempTargetResource + *indexes.begin();
			for (std::set<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); it++)
			{
				if (RegularFile(tempTargetResource + *it).valid())
				{
					return tempTargetResource + *it;
				}
			}
			tempTargetResource = defaultTargetResource;
		}
	}
	return tempTargetResource;
}

std::string Connection::configureTargetResource(std::string requestTarget)
{
	// =
	const std::vector<LocationConfig> &equalLocations = targetServer->getServerConfig().getLocations().at(0);
	std::vector<LocationConfig>::const_iterator selectedIt = equalLocations.end();

	for (std::vector<LocationConfig>::const_iterator it = equalLocations.begin(); it != equalLocations.end(); it++)
	{
		const std::string &pattern = it->getPattern();
		if (requestTarget == pattern)
		{
			targetLocationConfig = *it;
			return getTargetResource(requestTarget);
		}
	}

	// ^
	int depth = -1;
	const std::vector<LocationConfig> &locations = targetServer->getServerConfig().getLocations().at(2);
	selectedIt = locations.end();

	for (std::vector<LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); it++)
	{
		// deep route first
		const std::string &pattern = it->getPattern();
		if (requestTarget.find(pattern) == 0)
		{
			int currDepth = std::count(pattern.begin(), pattern.end(), '/');
			if (pattern.back() == '/')
				currDepth -= 1;
			if (depth < currDepth)
			{
				depth = currDepth;
				selectedIt = it;
			}
		}
	}
	targetLocationConfig = *selectedIt;
	return getTargetResource(requestTarget);
}

std::string Connection::getCGIExecutable()
{
	// $
	const std::vector<LocationConfig> &endLocations = targetServer->getServerConfig().getLocations().at(1);

	for (std::vector<LocationConfig>::const_iterator it = endLocations.begin(); it != endLocations.end(); it++)
	{
		const std::string &requestTarget = request.getRequestTarget().getTargetURI();
		const std::string &pattern = it->getPattern();
		if (requestTarget.rfind(pattern) == requestTarget.length() - pattern.length())
		{
			return (it->getCgiPath());
		}
	}
	return std::string();
}

void Connection::buildResponseFromRequest()
{
	if (statusCode)
	{
		buildErrorResponse(statusCode);
		response.setResponseBuffer();
	}
	else
	{
		std::cout << std::endl << "TARGET\n" << targetResource << std::endl;
		std::string method = request.getMethod();

		response.setStatusLine("HTTP/1.1 200 OK");
		response.addToHeaders("Server", "Hafserv/1.0.0");

		if (!targetLocationConfig.isAllowedMethod(method))
		{
			buildErrorResponse(405);
			response.setResponseBuffer();
			return;
		}

		std::string cgiExecutable = getCGIExecutable();
		if (cgiExecutable.size()) // CGI exists
		{
			File cgiFile(cgiExecutable);

			if (!cgiFile.isExecutable())
			{
				statusCode = 500;
				buildErrorResponse(statusCode);
				response.setResponseBuffer();
				return;
			}
			buildCGIResponse(cgiExecutable);
		}
		else // non CGI
		{
			if (method == "GET")
				buildGetResponse();
			else if (method == "POST")
				buildPostResponse();
			else if (method == "DELETE")
				buildDeleteResponse();
			response.setResponseBuffer();
		}
	}
}

void Connection::buildDeleteResponse()
{
	File targetFile(targetResource);

	if (!targetFile.isRegularFile())
		buildErrorResponse(404);
	else
	{
		if (!std::remove(targetResource.c_str())) // delete success
			response.setBody("<html><body><h1>File" + targetResource + " deleted.</h1></body></html>");
		else // delete fails
			buildErrorResponse(403);
	}
}

void Connection::buildGetResponse()
{
	File targetFile(targetResource);

	if (targetLocationConfig.getProxyPass().length() != 0)
		buildRedirectResponse(targetLocationConfig.getProxyPass() + request.getRequestTarget().getTargetURI());
	else
	{
		if (!targetFile.exist())
			buildErrorResponse(404);
		else if (targetFile.isDirectory())
		{
			if (targetLocationConfig.getAutoIndex() && targetResource.back() == '/')
				buildDirectoryResponse();
			else
				buildRedirectResponse("http://" + request.getHeaders().find("host")->second +
									  request.getRequestTarget().getTargetURI() + "/");
		}
		else if (!targetFile.isReadable() || targetFile.getFileSize() > 2147483648)
			buildErrorResponse(403);
		else
			response.makeBody(targetLocationConfig, targetResource);
	}
}

void Connection::buildPostResponse()
{
	File targetFile(targetResource);

	if (!targetFile.exist())
		buildErrorResponse(404);
	else if (targetFile.isDirectory())
	{
		if (targetLocationConfig.getAutoIndex() && targetResource.back() == '/')
			buildDirectoryResponse();
		else
			buildRedirectResponse("http://" + request.getHeaders().find("host")->second +
								  request.getRequestTarget().getTargetURI() + "/");
	}
	else if (!targetFile.isReadable() || targetFile.getFileSize() > 2147483648)
		buildErrorResponse(403);
	else
		response.makeBody(targetLocationConfig, targetResource);
}

void Connection::buildDirectoryResponse()
{
	Directory dir(targetResource);
	response.setBody(dir.toHtml());
	response.addToHeaders("Content-Type", "text/html; charset=utf-8");
}

void Connection::buildErrorResponse(int statusCode)
{
	std::string startLine = "HTTP/1.1 " + Webserv::getInstance().getStatusMessage(statusCode);
	response.setStatusLine(startLine);
	// best matching LocationConfig == targetLocationConfig
	std::map<int, std::string> errorPages = targetLocationConfig.getErrorPages();
	// found error page
	std::map<int, std::string>::iterator targetIt = errorPages.find(statusCode);
	// targetResource
	if (targetIt == errorPages.end())
		response.makeErrorBody(statusCode);
	else
	{
		std::string targetResource = configureTargetResource(targetIt->second);
		if (!File(targetResource).isReadable())
			response.makeErrorBody(statusCode);
		else
			response.makeBody(targetLocationConfig, targetResource);
	}
	if (statusCode == 413)
		response.removeHeaders("Content-Length");
}

void Connection::buildRedirectResponse(const std::string &redirectTarget)
{
	std::string startLine = "HTTP/1.1 301 Moved Permanently";
	response.setStatusLine(startLine);
	response.addToHeaders("Location", redirectTarget);
	std::map<int, std::string> errorPages = targetLocationConfig.getErrorPages();
	std::map<int, std::string>::iterator targetIt = errorPages.find(301);
	if (targetIt == errorPages.end())
		response.makeErrorBody(301);
	else
	{
		std::string targetResource = configureTargetResource(targetIt->second);
		response.makeBody(targetLocationConfig, targetResource);
	}
}

void Connection::buildCGIResponse(const std::string &scriptPath)
{
	int outward_fd[2];
	int inward_fd[2];

	response.setResponseState(Response::BuildingCGI);
	if (pipe(outward_fd) == -1)
	{
		buildErrorResponse(500);
		response.setResponseBuffer();
		return;
	}
	if (pipe(inward_fd) == -1)
	{
		buildErrorResponse(500);
		close(outward_fd[0]);
		close(outward_fd[1]);
		response.setResponseBuffer();
		return;
	}

	if (fcntl(outward_fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 ||
		fcntl(outward_fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 ||
		fcntl(inward_fd[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1 ||
		fcntl(inward_fd[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
	{
		buildErrorResponse(500);
		close(outward_fd[0]);
		close(outward_fd[1]);
		close(inward_fd[0]);
		close(inward_fd[1]);
		response.setResponseBuffer();
		return;
	}
	cgiPID = fork();
	if (cgiPID == -1)
	{
		buildErrorResponse(500);
		close(outward_fd[0]);
		close(outward_fd[1]);
		close(inward_fd[0]);
		close(inward_fd[1]);
		response.setResponseBuffer();
		return;
	}
	if (cgiPID == 0)
	{
		char *argv[] = {(char *)scriptPath.c_str(), (char *)targetResource.c_str(), NULL};
		char **envp = makeEnvp();
		close(outward_fd[0]);
		close(inward_fd[1]);
		dup2(outward_fd[1], STDOUT_FILENO);
		dup2(inward_fd[0], STDIN_FILENO);
		execve(argv[0], argv, envp);
		std::exit(1);
	}
	else
	{
		close(outward_fd[1]);
		close(inward_fd[0]);

		response.setResponseState(Response::BuildingCGI);
		wrBytes = request.getBody().length();
		written = 0;
		readen = 0;

		if (wrBytes > 0)
		{
			readPipe = outward_fd[0];
			writePipe = inward_fd[1];
		}
		else
		{
			readPipe = outward_fd[0];
			close(inward_fd[1]);
			writePipe = 0;
		}
		Webserv::getInstance().addCGIEvent(socket, readPipe, writePipe);
	}
}

void Connection::writeToCGI(int fd)
{
	const char *wrBuffer = request.getBody().c_str();
	wrBytes = request.getBody().length();
	int bytesToWrite = wrBytes - written;
	if (bytesToWrite > BUFFER_SIZE)
		bytesToWrite = BUFFER_SIZE;
	if (!bytesToWrite)
		return;
	int status = 0;
	if (waitpid(cgiPID, &status, WNOHANG) == 0)
	{
		int ret = write(fd, wrBuffer + written, bytesToWrite);
		if (ret > 0)
			written += ret;
		else
		{
			Webserv::getInstance().deleteCGIEvent(readPipe, writePipe);
			response.setResponseState(Response::End);
			connectionClose = true;
			return;
		}
	}
	else
	{
		Webserv::getInstance().deleteCGIEvent(readPipe, writePipe);
		response.setResponseState(Response::End);
		connectionClose = true;
		return;
	}
}

void Connection::readFromCGI(int fd)
{
	ssize_t bytes_read;
	char buffer[BUFFER_SIZE + 1];

	bytes_read = read(fd, buffer, BUFFER_SIZE);
	if (bytes_read < 0)
	{
		Webserv::getInstance().deleteCGIEvent(readPipe, writePipe);
		response.setResponseState(Response::End);
		connectionClose = true;
	}
	else if (bytes_read > 0)
	{
		CGIOutput.write(buffer, bytes_read);
		readen += bytes_read;
	}
	int status;

	if (waitpid(cgiPID, &status, WNOHANG) > 0)
		end = true;
	if (end && !bytes_read)
	{
		Webserv::getInstance().deleteCGIEvent(readPipe, writePipe);
		std::string header;

		while (true)
		{
			std::getline(CGIOutput, header);
			if (!header.size() || header == "\r")
				break;
			if (header.back() == '\r')
				header.erase(header.length() - 1);
			std::istringstream iss(header);
			std::string key, value;
			std::getline(iss, key, ':');
			std::getline(iss >> std::ws, value);
			if (util::string::toLower(key) == "status")
				response.setStatusLine("HTTP/1.1 " + value);
			else
				response.addToHeaders(key, value);
		}
		std::ostringstream body;
		body << CGIOutput.rdbuf();
		response.setBody(body.str());
		response.setResponseBuffer();
	}
	if (WIFEXITED(status) && WEXITSTATUS(status))
	{
		Webserv::getInstance().deleteCGIEvent(readPipe, writePipe);
		response.setResponseState(Response::End);
		connectionClose = true;
	}
}

char **Connection::makeEnvp()
{
	std::vector<std::string> envVec;
	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("PATH_INFO=/" + targetResource);
	envVec.push_back("QUERY_STRING=" + request.getRequestTarget().getQueryString());
	std::string requestMethod("REQUEST_METHOD=");
	requestMethod += request.getMethod();
	envVec.push_back(requestMethod);

	HeaderMultiMap::const_iterator it = request.getHeaders().begin();
	for (; it != request.getHeaders().end(); it++)
	{
		std::string key = (*it).first;
		std::string value = (*it).second;
		for (size_t i = 0; i < key.size(); i++)
		{
			key[i] = toupper(key[i]);
			if (key[i] == '-')
				key[i] = '_';
		}
		if (key.find("X_") == 0)
			key = "HTTP_" + key;
		envVec.push_back(key + "=" + value);
	}

	std::stringstream ss;
	ss << request.getBody().length();
	std::string contentLength("CONTENT_LENGTH=");
	contentLength += ss.str();
	envVec.push_back(contentLength);
	char **envp = new char *[envVec.size() + 1];
	for (size_t i = 0; i < envVec.size(); i++)
	{
		const char *envString = envVec[i].c_str();
		char *env = new char[envVec[i].length() + 1];
		std::strcpy(env, envString);
		envp[i] = env;
	}
	envp[envVec.size()] = NULL;
	return envp;
}

void Connection::sendResponse()
{
	startTime = time(NULL);
	if (!response.send(socket))
		connectionClose = true;
}

void Connection::reset()
{
	startTime = time(NULL);
	statusCode = 0;
	targetServer = NULL;
	targetResource = "";
	targetLocationConfig = LocationConfig();
	request = Request();
	response = Response();
	CGIOutput.str("");
	CGIOutput.clear();
}

int Connection::getStatusCode() const { return statusCode; }

const Request &Connection::getRequest() const { return request; }

const Response &Connection::getResponse() const { return response; }

const Server *Connection::getTargetServer() const { return targetServer; }

const time_t &Connection::getStartTime() const { return startTime; }

const LocationConfig &Connection::getTargetLocationConfig() const { return targetLocationConfig; }

int Connection::getReadPipe() const { return readPipe; }

int Connection::getWritePipe() const { return writePipe; }

int Connection::getConnectionClose() const { return connectionClose; }

pid_t Connection::getCGIPid() const { return cgiPID; }
