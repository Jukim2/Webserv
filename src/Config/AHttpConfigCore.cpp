#include "Config/AHttpConfigCore.hpp"

using namespace Hafserv;

AHttpConfigCore::Timeout::Timeout() : clientHeader(60), clientBody(60), keepAlive(75), send(60) {}

AHttpConfigCore::AHttpConfigCore()
	: allowMethods(), autoIndex(false), root("html"), indexes(), errorPages(), timeouts(), defaultType("text/plain"),
	  types(), clientMaxBodySize(2147483647)

{
}

AHttpConfigCore::AHttpConfigCore(const AHttpConfigCore &other)
	: allowMethods(other.allowMethods), autoIndex(other.autoIndex), root(other.root), indexes(other.indexes),
	  errorPages(other.errorPages), timeouts(other.timeouts), defaultType(other.defaultType), types(other.types),
	  clientMaxBodySize(other.clientMaxBodySize)
{
}

AHttpConfigCore &AHttpConfigCore::operator=(const AHttpConfigCore &other)
{
	if (this != &other)
	{
		clientMaxBodySize = other.clientMaxBodySize;
		autoIndex = other.autoIndex;
		root = other.root;
		indexes = other.indexes;
		timeouts = other.timeouts;
		errorPages = other.errorPages;
		defaultType = other.defaultType;
		types = other.types;
		allowMethods = other.allowMethods;
	}
	return *this;
}

AHttpConfigCore::~AHttpConfigCore() {}

int AHttpConfigCore::getClientMaxBodySize() const { return clientMaxBodySize; }
bool AHttpConfigCore::getAutoIndex() const { return autoIndex; }
const std::string &AHttpConfigCore::getRoot() const { return root; }
const std::set<std::string> &AHttpConfigCore::getIndexes() const { return indexes; }
const AHttpConfigCore::Timeout &AHttpConfigCore::getTimeout() const { return timeouts; }
const std::map<int, std::string> &AHttpConfigCore::getErrorPages() const { return errorPages; }
const std::string &AHttpConfigCore::getDefaultType() const { return defaultType; }
const std::multimap<std::string, std::string> &AHttpConfigCore::getTypes() const { return types; }
const std::set<std::string> &AHttpConfigCore::getAllowMethods() const { return allowMethods; }

void AHttpConfigCore::setAllowMethods(const std::set<std::string> &allowMethods) { this->allowMethods = allowMethods; }
void AHttpConfigCore::setAutoIndex(const bool autoIndex) { this->autoIndex = autoIndex; }
void AHttpConfigCore::setRoot(const std::string &root) { this->root = root; }
void AHttpConfigCore::setIndexes(const std::set<std::string> &indexes) { this->indexes = indexes; }
void AHttpConfigCore::setErrorPages(const std::map<int, std::string> &errorPages) { this->errorPages = errorPages; }
void AHttpConfigCore::setTimeouts(const Timeout &timeouts) { this->timeouts = timeouts; }
void AHttpConfigCore::setClientHeaderTimeout(int timeout) { this->timeouts.clientHeader = timeout; }
void AHttpConfigCore::setClientBodyTimeout(int timeout) { this->timeouts.clientBody = timeout; }
void AHttpConfigCore::setKeepAliveTimeout(int timeout) { this->timeouts.keepAlive = timeout; }
void AHttpConfigCore::setSendTimeout(int timeout) { this->timeouts.send = timeout; }
void AHttpConfigCore::setDefaultType(const std::string &defaultType) { this->defaultType = defaultType; }
void AHttpConfigCore::setTypes(const std::multimap<std::string, std::string> &types) { this->types = types; }
void AHttpConfigCore::setClientMaxBodySize(const int clientMaxBodySize) { this->clientMaxBodySize = clientMaxBodySize; }

void AHttpConfigCore::addErrorPage(int errorCode, const std::string &uri)
{
	errorPages.insert(std::make_pair(errorCode, uri));
}

bool AHttpConfigCore::addIndex(const std::string &index) { return indexes.insert(index).second; }

void AHttpConfigCore::addType(const std::string &type, const std::string &extension)
{
	types.insert(std::make_pair(type, extension));
}

bool AHttpConfigCore::addAllowMethod(const std::string &method) { return allowMethods.insert(method).second; }

bool AHttpConfigCore::isAllowedMethod(const std::string &method) const
{
	if (allowMethods.empty() && method == "GET")
		return true;
	else
		return allowMethods.count(method) > 0;
}

void AHttpConfigCore::setHttpConfigCore(const ConfigFile::directives_t &directives) throw(std::logic_error)
{
	std::vector<std::string> params;
	size_t numToken;
	std::pair<int, bool> valueToInt;
	bool hasRoot = false;

	ConfigFile::directives_t::const_iterator it = directives.begin();
	for (; it != directives.end(); it++)
	{
		const std::string &key = (*it).first;
		const std::string &value = (*it).second;

		params = util::string::split(value, ' ');
		numToken = params.size();
		valueToInt = util::string::stoi(value);

		if (key == "client_max_body_size")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw InvalidArgumentError(key);

			clientMaxBodySize = valueToInt.first;
		}
		else if (key == "autoindex")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);

			if (value == "on")
				autoIndex = true;
			else if (value == "off")
				autoIndex = false;
			else
				throw InvalidArgumentError(key, value);
		}
		else if (key == "root")
		{
			if (hasRoot)
				throw DuplicateDirectiveError(key);
			else if (numToken != 1)
				throw InvalidNumberArgumentError(key);

			root = value;
			hasRoot = true;
		}
		else if (key == "index")
		{
			std::vector<std::string> newIndexes;
			for (size_t i = 0; i < numToken; i++)
				addIndex(params[i]);
		}
		else if (key == "client_header_timeout")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw InvalidArgumentError(key);

			timeouts.clientHeader = valueToInt.first;
		}
		else if (key == "client_body_timeout")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw InvalidArgumentError(key);

			timeouts.clientBody = valueToInt.first;
		}
		else if (key == "keepalive_timeout")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw InvalidArgumentError(key);

			timeouts.keepAlive = valueToInt.first;
		}
		else if (key == "send_timeout")
		{
			if (numToken != 1)
				throw InvalidNumberArgumentError(key);
			else if (!valueToInt.second)
				throw InvalidArgumentError(key);

			timeouts.send = valueToInt.first;
		}
		else if (key == "error_page")
		{
			if (params.size() == 1)
				throw InvalidArgumentError(key, value);
			for (size_t i = 0; i < params.size() - 1; i++)
			{
				valueToInt = util::string::stoi(params[i]);
				if (valueToInt.second && 300 <= valueToInt.first && valueToInt.first < 600)
					errorPages.insert(std::make_pair(valueToInt.first, params.back()));
				else
					throw InvalidErrorCodeError(params[i]);
			}
		}
		else if (key == "allow_methods")
		{
			for (size_t i = 0; i < numToken; i++)
			{
				if (params[i] != "GET" && params[i] != "POST" && params[i] != "DELETE")
					throw InvalidArgumentError(key, value);
				addAllowMethod(params[i]);
			}
		}
	}
}

void AHttpConfigCore::setHttpConfigCore(const ConfigFile::subblocks_t &subBlocks) throw()
{
	std::string subBlockName, type, extension;
	for (size_t i = 0; i < subBlocks.size(); i++)
	{
		subBlockName = subBlocks.at(i).getBlockDirective();
		if (subBlockName == "types")
		{
			ConfigFile::directives_t::const_iterator it = subBlocks.at(i).getDirectives().begin();
			for (; it != subBlocks.at(i).getDirectives().end(); it++)
			{
				type = (*it).first;
				extension = (*it).second;
				std::vector<std::string> extensions = util::string::split(extension, ' ');
				for (size_t j = 0; j < extensions.size(); j++)
					addType(extensions.at(j), type);
			}
		}
	}
}

std::ostream &operator<<(std::ostream &os, const AHttpConfigCore &conf)
{
	os << "\t[AHttpConfigCore]" << std::endl;
	os << "\t\tallow methods: ";
	if (conf.getAllowMethods().size())
	{
		for (std::set<std::string>::const_iterator it = conf.getAllowMethods().begin();
			 it != conf.getAllowMethods().end(); ++it)
			os << *it << ", ";
		os << std::endl;
	}
	else
		os << "GET" << std::endl;
	os << "\t\tautoindex: " << (conf.getAutoIndex() ? "on" : "off") << std::endl;
	os << "\t\troot: " << conf.getRoot() << std::endl;
	os << "\t\tindexes: ";
	for (std::set<std::string>::const_iterator it = conf.getIndexes().begin(); it != conf.getIndexes().end(); ++it)
		os << (*it) << " ";
	os << std::endl;

	// errorPages
	if (conf.getErrorPages().size())
	{
		os << "\t\terror code: ";
		std::map<int, std::string>::const_iterator it = conf.getErrorPages().begin();
		for (; it != conf.getErrorPages().end(); it++)
			os << (*it).first << "=" << (*it).second << ", ";
		if (conf.getErrorPages().size())
			os << std::endl;
	}

	os << conf.getTimeout();
	os << std::endl;

	os << "\t\tdefault_types: " << conf.getDefaultType() << std::endl;
	// types
	if (conf.getTypes().size())
	{
		os << "\t\textension: ";
		size_t i = 0;
		std::map<std::string, std::string>::const_iterator it2 = conf.getTypes().begin();
		for (; it2 != conf.getTypes().end() && i < 3; ++it2)
		{
			os << (*it2).first << "=" << (*it2).second << ", ";
			i++;
		}
		if (it2 != conf.getTypes().end())
			os << " ..." << std::endl;
		else
			os << std::endl;
	}
	os << "\t\tclient_max_body_size: " << conf.getClientMaxBodySize() << std::endl;

	return os;
}

std::ostream &operator<<(std::ostream &os, const AHttpConfigCore::Timeout &timeouts)
{
	os << "\t\ttimeouts: clientHeader=" << timeouts.clientHeader;
	os << ", clientBody=" << timeouts.clientBody;
	os << ", keepalive=" << timeouts.keepAlive;
	os << ", send=" << timeouts.send;
	return os;
}
