#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config/ServerConfig.hpp"
#include <set>
#include <string>

namespace Hafserv
{
class Server
{
public:
	Server(ServerConfig serverConfig);
	~Server();

	const std::set<unsigned short> &getPorts() const;
	const std::set<std::string> &getNames() const;
	const ServerConfig &getServerConfig() const;
	/*
		string -> Request
		parseRequest
		makeResponse
		sendResponse
	*/
private:
	ServerConfig serverConfig;
};
} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::Server &server);

#endif
