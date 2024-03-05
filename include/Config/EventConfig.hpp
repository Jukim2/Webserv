#ifndef EVENTCONFIG_HPP
#define EVENTCONFIG_HPP

#include "Config/AConfig.hpp"
#include "File/ConfigFile.hpp"

#include <iostream>

namespace Hafserv
{
class EventConfig : public AConfig
{
public:
	EventConfig();
	EventConfig(const EventConfig &other);
	EventConfig(const ConfigFile &block) throw(std::logic_error);
	EventConfig &operator=(const EventConfig &other);
	~EventConfig();

	int getWorkerConnections() const;

private:
	int workerConnections;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::EventConfig &conf);

#endif
