#include "Config/EventConfig.hpp"

#include "Config/ConfigException.hpp"
#include "util/string.hpp"

using namespace Hafserv;

EventConfig::EventConfig() : AConfig(), workerConnections(512) {}

EventConfig::EventConfig(const EventConfig &other) : AConfig(other), workerConnections(other.workerConnections) {}

EventConfig::EventConfig(const ConfigFile &block) throw(std::logic_error) : AConfig(), workerConnections(512)
{
	std::vector<std::string> params;
	size_t numToken;
	std::pair<int, bool> valueToInt;

	ConfigFile::directives_t::const_iterator it = block.getDirectives().begin();
	for (; it != block.getDirectives().end(); it++)
	{
		const std::string &key = (*it).first;
		const std::string &value = (*it).second;

		params = util::string::split(value, ' ');
		numToken = params.size();
		valueToInt = util::string::stoi(value);

		if (allBlockDirectives.count(key))
			throw NoBraceError(key);
		else if (!allSimpleDirectives.count(key))
			throw UnknownDirectiveError(key);
		else if (!eventsSimpleDirectives.count(key))
			throw DisallowDirectiveError(key);

		if (key == "worker_connections")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw ParseError("stoi failed: " + value);

			workerConnections = valueToInt.first;
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
	}
}

EventConfig &EventConfig::operator=(const EventConfig &other)
{
	if (this != &other)
	{
		AConfig::operator=(other);
		workerConnections = other.workerConnections;
	}
	return *this;
}

EventConfig::~EventConfig() {}

int EventConfig::getWorkerConnections() const { return workerConnections; }

std::ostream &operator<<(std::ostream &os, const EventConfig &conf)
{
	os << "[EventConfig]" << std::endl;

	os << "workerConnections: " << conf.getWorkerConnections() << std::endl;
	return os;
}
