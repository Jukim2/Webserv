#include "Config/HttpConfig.hpp"

#include "Config/ConfigException.hpp"

using namespace Hafserv;

HttpConfig::HttpConfig() : AConfig(), AHttpConfigCore(), servers() {}

HttpConfig::HttpConfig(const HttpConfig &other) : AConfig(other), AHttpConfigCore(other), servers(other.servers) {}

HttpConfig::HttpConfig(const ConfigFile &block) throw(std::logic_error) : AConfig(), AHttpConfigCore(), servers()
{
	setHttpConfigCore(block.getDirectives());
	setHttpConfigCore(block.getSubBlocks());

	ConfigFile::directives_t::const_iterator it = block.getDirectives().begin();
	for (; it != block.getDirectives().end(); it++)
	{
		const std::string &key = (*it).first;

		if (allBlockDirectives.count(key))
			throw NoBraceError(key);
		else if (!allSimpleDirectives.count(key))
			throw UnknownDirectiveError(key);
		else if (!httpSimpleDirectives.count(key))
			throw DisallowDirectiveError(key);
	}

	for (size_t i = 0; i < block.getSubBlocks().size(); i++)
	{
		const ConfigFile &subBlock = block.getSubBlocks().at(i);
		const std::string &subBlockName = subBlock.getBlockDirective();

		if (allSimpleDirectives.count(subBlockName))
			throw NoSemicolonError(subBlockName);
		else if (!allBlockDirectives.count(subBlockName))
			throw UnknownDirectiveError(subBlockName);
		else if (!httpBlockDirectives.count(subBlockName))
			throw DisallowDirectiveError(subBlockName);

		if (subBlockName == "server")
		{
			ServerConfig newServerConfig = ServerConfig(block.getSubBlocks().at(i), *this);

			bool isDuplicated = false;
			std::set<std::string>::const_iterator it_names;
			std::set<unsigned short>::const_iterator it_ports;

			it_names = newServerConfig.getNames().begin();
			for (; it_names != newServerConfig.getNames().end(); ++it_names)
			{
				it_ports = newServerConfig.getPorts().begin();
				for (; it_ports != newServerConfig.getPorts().end(); ++it_ports)
				{
					for (size_t j = 0; j < servers.size(); ++j)
					{
						if (servers[j].hasName(*it_names) && servers[j].hasPort(*it_ports))
						{
							std::cout << "warning: conflicting server name \"" + *it_names;
							std::cout << "\" on 0.0.0.0:" + util::string::itos(*it_ports) + ", ignored" << std::endl;
							isDuplicated = true;
						}
					}
				}
			}

			if (!isDuplicated)
				servers.push_back(newServerConfig);
		}
	}

	if (servers.empty())
		throw ParseError("server block not found");
}

HttpConfig &HttpConfig::operator=(const HttpConfig &other)
{
	if (this != &other)
	{
		AConfig::operator=(other);
		AHttpConfigCore::operator=(other);
		servers = other.servers;
	}
	return *this;
}

HttpConfig::~HttpConfig() {}

const std::vector<ServerConfig> &HttpConfig::getServers() const { return servers; }

std::ostream &operator<<(std::ostream &os, const HttpConfig &conf)
{
	os << "[HttpConfig]" << std::endl;

	const AHttpConfigCore &temp = conf;
	os << temp << std::endl;

	for (size_t i = 0; i < conf.getServers().size(); i++)
		os << conf.getServers().at(i) << std::endl;
	return os;
}
