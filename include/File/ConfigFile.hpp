#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include "Config/ConfigException.hpp"
#include "File/RegularFile.hpp"

#include <map>
#include <string>
#include <vector>

namespace Hafserv
{
class ConfigFile : public RegularFile
{
public:
	typedef std::vector<std::string> parameters_t;
	typedef std::multimap<std::string, std::string> directives_t;
	typedef std::vector<ConfigFile> subblocks_t;

	ConfigFile();
	ConfigFile(const ConfigFile &other);
	ConfigFile(const RegularFile &other);
	ConfigFile(const std::string &name) throw(ParseError);
	ConfigFile &operator=(const ConfigFile &other);
	virtual ~ConfigFile() throw();

	const std::string &getBlockDirective() const;
	const parameters_t &getParameters() const;
	const directives_t &getDirectives() const;
	const subblocks_t &getSubBlocks() const;

	void include() throw(IncludeError, ParseError);

private:
	static const std::string meta;

	std::string blockDirective;
	std::vector<std::string> parameters;
	std::multimap<std::string, std::string> directives;
	std::vector<ConfigFile> subBlocks;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::ConfigFile &configFile);

#endif
