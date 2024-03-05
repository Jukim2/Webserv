#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include "Config/AConfig.hpp"
#include "Config/AHttpConfigCore.hpp"
#include "Config/ServerConfig.hpp"
#include "File/ConfigFile.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace Hafserv
{
class HttpConfig : public AConfig, public AHttpConfigCore
{
public:
	HttpConfig();
	HttpConfig(const HttpConfig &other);
	HttpConfig(const ConfigFile &block) throw(std::logic_error);
	HttpConfig &operator=(const HttpConfig &other);
	~HttpConfig();

	const std::vector<ServerConfig> &getServers() const;

private:
	std::vector<ServerConfig> servers;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::HttpConfig &conf);

#endif
