#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Webserv.hpp"

namespace Hafserv
{

#define BUFFER_SIZE 100000
#define SYS_ERROR -1

class Webserv;

class Connection
{
public:
	Connection(int socket, unsigned short port);
	Connection(const Connection &other);
	Connection &operator=(Connection &rhs);
	~Connection();

	void readRequest(int fd);
	std::string configureTargetResource(std::string requestTarget);
	std::string getTargetResource(std::string &requestTarget);

	void buildResponseFromRequest();
	void buildGetResponse();
	void buildPostResponse();
	void buildDirectoryResponse();
	void buildDeleteResponse();
	void buildErrorResponse(int statusCode);
	void buildRedirectResponse(const std::string &redirectTarget);

	std::string getCGIExecutable();
	void buildCGIResponse(const std::string &scriptPath);
	void writeToCGI(int fd);
	void readFromCGI(int fd);
	char **makeEnvp();

	void sendResponse();
	void reset();

	int getStatusCode() const;
	const Request &getRequest() const;
	const Response &getResponse() const;
	const Server *getTargetServer() const;
	const time_t &getStartTime() const;
	const LocationConfig &getTargetLocationConfig() const;
	int getReadPipe() const;
	int getWritePipe() const;
	int getConnectionClose() const;
	pid_t getCGIPid() const;

private:
	Request request;
	Response response;
	int socket;
	unsigned short port;

	int statusCode;
	time_t startTime;
	Server *targetServer;
	LocationConfig targetLocationConfig;
	std::string targetResource;

	pid_t cgiPID;
	int readPipe;
	int writePipe;
	size_t wrBytes;
	size_t written;
	std::stringstream CGIOutput;
	size_t readen;
	bool end;
	bool connectionClose;
};

} // namespace Hafserv
#endif
