#include "Server.hpp"
#include "Http/Response.hpp"

using namespace Hafserv;

Server::Server(ServerConfig serverConfig) : serverConfig(serverConfig) {}

Server::~Server() {}

const std::set<unsigned short> &Server::getPorts() const
{
    return serverConfig.getPorts();
}

const std::set<std::string> &Server::getNames() const
{
    return serverConfig.getNames();
}

const ServerConfig &Server::getServerConfig() const { return serverConfig; }

std::ostream &operator<<(std::ostream &os, const Hafserv::Server &server)
{
    os << "Server Info" << std::endl;
    os << server.getServerConfig();
    return os;
}
