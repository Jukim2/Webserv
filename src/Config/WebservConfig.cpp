#include "Config/WebservConfig.hpp"

#include "Config/ConfigException.hpp"

using namespace Hafserv;

WebservConfig::WebservConfig() : AConfig(), event(), http() {}

WebservConfig::WebservConfig(const WebservConfig &other) : AConfig(other), event(other.event), http(other.http) {}

WebservConfig::WebservConfig(const ConfigFile &configFile) throw(std::logic_error) : AConfig(), event(), http()
{
	ConfigFile main = configFile.getSubBlocks().at(0);

	ConfigFile::directives_t::const_iterator it = main.getDirectives().begin();
	for (; it != main.getDirectives().end(); it++)
	{
		const std::string &key = (*it).first;

		if (allBlockDirectives.count(key))
			throw NoBraceError(key);
		else if (!allSimpleDirectives.count(key))
			throw UnknownDirectiveError(key);
		else if (!mainSimpleDirectives.count(key))
			throw DisallowDirectiveError(key);
	}

	bool hasEvent = false, hasHttp = false;
	for (size_t i = 0; i < main.getSubBlocks().size(); i++)
	{
		const ConfigFile &subBlock = main.getSubBlocks().at(i);
		const std::string &subBlockName = subBlock.getBlockDirective();

		if (allSimpleDirectives.count(subBlockName))
			throw NoSemicolonError(subBlockName);
		else if (!allBlockDirectives.count(subBlockName))
			throw UnknownDirectiveError(subBlockName);
		else if (!mainBlockDirectives.count(subBlockName))
			throw DisallowDirectiveError(subBlockName);

		if (subBlockName == "events")
		{
			if (hasEvent)
				throw DuplicateDirectiveError("events");
			hasEvent = true;
			event = EventConfig(subBlock);
		}
		else if (subBlockName == "http")
		{
			if (hasHttp)
				throw DuplicateDirectiveError("http");
			hasHttp = true;
			http = HttpConfig(subBlock);
		}
	}
	if (!hasHttp)
		throw ParseError("http block not found");
}

WebservConfig &WebservConfig::operator=(const WebservConfig &other)
{
	if (this != &other)
	{
		AConfig::operator=(other);
		event = other.event;
		http = other.http;
	}
	return *this;
}

WebservConfig::~WebservConfig() {}

const HttpConfig &WebservConfig::getHttpConfig() const { return http; }

const EventConfig &WebservConfig::getEventConfig() const { return event; }

std::ostream &operator<<(std::ostream &os, const WebservConfig &conf)
{
	os << "[WebservConfig]" << std::endl << std::endl;
	os << conf.getEventConfig();
	os << std::endl;
	os << conf.getHttpConfig();
	return os;
}
