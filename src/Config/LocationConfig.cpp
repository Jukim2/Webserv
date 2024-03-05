#include "Config/LocationConfig.hpp"

#include "Config/ConfigException.hpp"

using namespace Hafserv;

LocationConfig::LocationConfig() : AConfig(), AHttpConfigCore(), modifier(), pattern(), alias(), proxyPass(), cgiPath()
{
}

LocationConfig::LocationConfig(const LocationConfig &other)
	: AConfig(other), AHttpConfigCore(other), modifier(other.modifier), pattern(other.pattern), alias(other.alias),
	  proxyPass(other.proxyPass), cgiPath(other.cgiPath)
{
}

LocationConfig::LocationConfig(const AHttpConfigCore &core)
	: AConfig(), AHttpConfigCore(core), modifier(), pattern(), alias(), proxyPass(), cgiPath()
{
}

LocationConfig::LocationConfig(const ConfigFile &block, const AHttpConfigCore &core) throw(std::logic_error)
	: AConfig(), AHttpConfigCore(core), modifier(), pattern(), alias(), proxyPass(), cgiPath()
{
	/* SAMPLE LOCATION BLOCK
	 * location modifier(=, $, ^) pattern {
			# root path; 	// can exist with no alias
			alias path; 	// can exist with no root
			proxy_path URL;
			cgi_path path;
	   }
	 */
	if (block.getParameters().size() != 2)
		throw ParseError("location parameter should be two.");
	modifier = block.getParameters().at(0);
	if (modifier != "=" && modifier != "$" && modifier != "^")
		throw ParseError("unexpected location parameter: " + modifier);
	pattern = block.getParameters().at(1);
	if (modifier == "^" && pattern.front() != '/')
		throw ParseError("pattern must start with '/'");

	setHttpConfigCore(block.getDirectives());
	setHttpConfigCore(block.getSubBlocks());

	size_t numToken;
	bool hasAlias = false, hasRoot = false, hasProxyPass = false, hasCgiPath = false;

	ConfigFile::directives_t::const_iterator it = block.getDirectives().begin();
	for (; it != block.getDirectives().end(); it++)
	{
		const std::string &key = (*it).first;
		const std::string &value = (*it).second;
		numToken = util::string::split(value, ' ').size();

		if (allBlockDirectives.count(key))
			throw NoBraceError(key);
		else if (!allSimpleDirectives.count(key))
			throw UnknownDirectiveError(key);
		else if (!locationSimpleDirectives.count(key))
			throw DisallowDirectiveError(key);

		if (key == "alias")
		{
			if (hasAlias)
				throw DuplicateDirectiveError(key);
			else if (hasRoot)
				throw DuplicateDirectiveError(key, "root");
			else if (numToken != 1)
				throw InvalidNumberArgumentError(key);

			this->alias = value;
			hasAlias = true;
		}
		else if (key == "proxy_pass")
		{
			if (hasProxyPass)
				throw DuplicateDirectiveError(key);

			hasProxyPass = true;
			this->proxyPass = value;
		}
		else if (key == "cgi_path")
		{
			if (hasCgiPath)
				throw DuplicateDirectiveError(key);
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			if (RegularFile(value).error())
				throw InvalidArgumentError(key, value);

			hasCgiPath = true;
			this->cgiPath = value;
		}
		else if (key == "root")
		{
			if (hasAlias)
				throw DuplicateDirectiveError(key, "alias");
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

LocationConfig &LocationConfig::operator=(const LocationConfig &other)
{
	if (this != &other)
	{
		AConfig::operator=(other);
		AHttpConfigCore::operator=(other);
		modifier = other.modifier;
		pattern = other.pattern;
		alias = other.alias;
		proxyPass = other.proxyPass;
		cgiPath = other.cgiPath;
	}
	return *this;
}

LocationConfig::~LocationConfig() {}

const std::string &LocationConfig::getModifier() const { return modifier; }

const std::string &LocationConfig::getPattern() const { return pattern; }

const std::string &LocationConfig::getAlias() const { return alias; }

const std::string &LocationConfig::getProxyPass() const { return proxyPass; }

const std::string &LocationConfig::getCgiPath() const { return cgiPath; }

void LocationConfig::setModifier(const std::string &modifier) { this->modifier = modifier; }

void LocationConfig::setPattern(const std::string &pattern) { this->pattern = pattern; }

void LocationConfig::setAlias(const std::string &alias) { this->alias = alias; }

void LocationConfig::setProxyPass(const std::string &proxyPass) { this->proxyPass = proxyPass; }

void LocationConfig::setCgiPath(const std::string &cgiPath) { this->cgiPath = cgiPath; }

bool LocationConfig::isMatching(const std::string &url)
{
	if (pattern.find(url) == 0)
		return true;
	else
		return false;
}

std::ostream &operator<<(std::ostream &os, const LocationConfig &conf)
{
	os << "[LocationConfig]" << std::endl;
	os << AHttpConfigCore(conf);
	os << "\tmodifier: " << conf.getModifier() << std::endl;
	os << "\tpattern: " << conf.getPattern() << std::endl;
	if (conf.getAlias().length())
		os << "\talias: " << conf.getAlias() << std::endl;
	else
		os << "\tno alias" << std::endl;
	if (conf.getProxyPass().length())
		os << "\tproxy_pass: " << conf.getProxyPass() << std::endl;
	else
		os << "\tno proxy_pass" << std::endl;
	if (conf.getCgiPath().length())
		os << "\tcgi_path: " << conf.getCgiPath();
	else
		os << "\tno cgi_pass";
	return os;
}
