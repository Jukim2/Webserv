#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Config/AConfig.hpp"
#include "Config/AHttpConfigCore.hpp"
#include "Config/LocationConfig.hpp"
#include "File/ConfigFile.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace Hafserv
{
class ServerConfig : public AConfig, public AHttpConfigCore
{
public:
	ServerConfig();
	ServerConfig(const ServerConfig &other);
	ServerConfig(const ConfigFile &block, const AHttpConfigCore &core) throw(std::logic_error);
	ServerConfig &operator=(const ServerConfig &other);
	~ServerConfig();

	const std::set<std::string> &getNames() const;
	const std::set<unsigned short> &getPorts() const;
	const std::vector<std::vector<LocationConfig> > &getLocations() const;

	bool hasName(const std::string &name) const;
	bool hasPort(const unsigned short port) const;

private:
	std::set<std::string> names;
	std::set<unsigned short> ports;
	std::vector<std::vector<LocationConfig> > locations;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::ServerConfig &conf);

#endif
