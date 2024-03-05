#include "Config/ServerConfig.hpp"

#include "Config/ConfigException.hpp"

using namespace Hafserv;

ServerConfig::ServerConfig() : AConfig(), AHttpConfigCore(), names(), ports(), locations(3) { ports.insert(80); }

ServerConfig::ServerConfig(const ServerConfig &other)
	: AConfig(other), AHttpConfigCore(other), names(other.names), ports(other.ports), locations(other.locations)
{
}

ServerConfig::ServerConfig(const ConfigFile &block, const AHttpConfigCore &core) throw(std::logic_error)
	: AConfig(), AHttpConfigCore(core), names(), ports(), locations(3)
{
	ports.insert(80);

	setHttpConfigCore(block.getDirectives());
	setHttpConfigCore(block.getSubBlocks());

	std::vector<std::string> params;
	size_t numToken;
	std::pair<unsigned short, bool> valueToUShort;

	ConfigFile::directives_t::const_iterator it = block.getDirectives().begin();
	for (; it != block.getDirectives().end(); it++)
	{
		const std::string &key = (*it).first;
		const std::string &value = (*it).second;

		params = util::string::split(value, ' ');
		numToken = params.size();
		valueToUShort = util::string::stous(value);

		if (allBlockDirectives.count(key))
			throw NoBraceError(key);
		else if (!allSimpleDirectives.count(key))
			throw UnknownDirectiveError(key);
		else if (!serverSimpleDirectives.count(key))
			throw DisallowDirectiveError(key);

		if (key == "listen")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToUShort.second)
				throw InvalidArgumentError(key, value);

			if (ports.size() == 1 && *ports.begin() == 80)
				ports.clear();
			if (!ports.insert(util::string::stous(value).first).second)
				throw DuplicateListenError(params[0]);
		}
		else if (key == "server_name")
		{
			for (size_t i = 0; i < params.size(); i++)
				names.insert(params[i]);
		}
	}

	for (size_t i = 0; i < block.getSubBlocks().size(); i++)
	{
		const ConfigFile &subBlock = block.getSubBlocks().at(i);
		const std::string &subBlockName = subBlock.getBlockDirective();

		if (allSimpleDirectives.count(subBlockName))
			throw NoSemicolonError(subBlockName);
		else if (!allBlockDirectives.count(subBlockName))
			throw UnknownDirectiveError(subBlockName);
		else if (!serverBlockDirectives.count(subBlockName))
			throw DisallowDirectiveError(subBlockName);

		if (subBlockName == "location")
		{
			LocationConfig conf = LocationConfig(block.getSubBlocks().at(i), *this);
			size_t mIdx = 0;
			if (conf.getModifier() == "=")
				mIdx = 0;
			else if (conf.getModifier() == "$")
				mIdx = 1;
			else if (conf.getModifier() == "^")
				mIdx = 2;
			for (size_t j = 0; j < locations[mIdx].size(); ++j)
				if (locations[mIdx][j].getPattern() == conf.getPattern())
					throw DuplicateLocationError(conf.getPattern());
			locations[mIdx].push_back(conf);
		}
	}

	bool hasDefault = false;
	for (size_t i = 0; i < locations[2].size(); ++i)
		if (locations[2][i].getPattern() == "/")
			hasDefault = true;
	if (!hasDefault)
	{
		LocationConfig defaultLocationConfig(*this);
		defaultLocationConfig.setModifier("^");
		defaultLocationConfig.setPattern("/");
		locations[2].push_back(defaultLocationConfig);
	}
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other)
{
	if (this != &other)
	{
		AConfig::operator=(other);
		AHttpConfigCore::operator=(other);
		names = other.names;
		ports = other.ports;
		locations = other.locations;
	}
	return *this;
}

ServerConfig::~ServerConfig() {}

const std::set<std::string> &ServerConfig::getNames() const { return names; }

const std::set<unsigned short> &ServerConfig::getPorts() const { return ports; }

const std::vector<std::vector<LocationConfig> > &ServerConfig::getLocations() const { return locations; }

bool ServerConfig::hasName(const std::string &name) const { return names.count(name) > 0; }

bool ServerConfig::hasPort(const unsigned short port) const { return ports.count(port) > 0; }

std::ostream &operator<<(std::ostream &os, const ServerConfig &conf)
{
	os << "[ServerConfig]" << std::endl;

	os << AHttpConfigCore(conf);

	os << "\tnames: ";
	for (std::set<std::string>::const_iterator it = conf.getNames().begin(); it != conf.getNames().end(); ++it)
		os << *it << " ";
	os << std::endl;

	os << "\tports: ";
	for (std::set<unsigned short>::const_iterator it = conf.getPorts().begin(); it != conf.getPorts().end(); ++it)
		os << static_cast<int>(*it) << " ";
	os << std::endl;

	for (size_t i = 0; i < conf.getLocations().size(); i++)
		for (size_t j = 0; j < conf.getLocations()[i].size(); j++)
			os << conf.getLocations()[i][j] << std::endl;
	return os;
}
