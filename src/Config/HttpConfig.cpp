/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyoon <gyoon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 22:46:15 by gyoon             #+#    #+#             */
/*   Updated: 2024/01/23 14:36:54 by gyoon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfig.hpp"

using namespace Hafserv;

HttpConfig::HttpConfig() : AHttpConfigModule(), directives(), servers() {}

HttpConfig::HttpConfig(const HttpConfig &other)
	: AHttpConfigModule(), directives(other.directives), servers(other.servers)
{
}

HttpConfig::HttpConfig(const ConfigFile &block) throw(ParseError)
	: AHttpConfigModule(), directives(block.directives), servers()
{
	this->setHttpConfigCore(block.directives);
	this->setHttpConfigCore(block.subBlocks);

	std::string subBlockName;
	for (size_t i = 0; i < block.subBlocks.size(); i++)
	{
		subBlockName = block.subBlocks.at(i).name;
		if (subBlockName == "server")
			servers.push_back(ServerConfig(block.subBlocks.at(i), core));
		else
			throw ParseError("unexpected http context block: " + subBlockName);
	}
}

HttpConfig &HttpConfig::operator=(const HttpConfig &other)
{
	if (this != &other)
	{
		core = other.core;
		directives = other.directives;
		servers = other.servers;
	}
	return *this;
}

HttpConfig::~HttpConfig() {}

const ConfigFile::directives_t &HttpConfig::getDirectives() const { return directives; }

const std::vector<ServerConfig> &HttpConfig::getServers() const { return servers; }

std::ostream &operator<<(std::ostream &os, const HttpConfig &conf)
{
	os << "[HttpConfig]" << std::endl;

	os << conf.getHttpConfigCore() << std::endl;

	ConfigFile::directives_t::const_iterator it = conf.getDirectives().begin();
	for (; it != conf.getDirectives().end(); it++)
		os << "\t[" << (*it).first << "] : [" << (*it).second << "]" << std::endl;

	os << std::endl;
	for (size_t i = 0; i < conf.getServers().size(); i++)
		os << conf.getServers().at(i) << std::endl;
	return os;
}