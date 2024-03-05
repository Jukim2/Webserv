#include "util/string.hpp"

using namespace Hafserv;

std::pair<int, bool> util::string::stoi(const std::string &str)
{
	std::pair<int, bool> ret = std::make_pair(0, false);
	std::stringstream ss(str);
	ss >> ret.first;
	ret.second = !ss.fail() && !ss.bad() && ss.eof();
	return ret;
}

std::pair<unsigned short, bool> util::string::stous(const std::string &str)
{
	std::pair<unsigned short, bool> ret = std::make_pair(0, false);
	std::stringstream ss(str);
	ss >> ret.first;
	ret.second = !ss.fail() && !ss.bad() && ss.eof();
	return ret;
}

std::string util::string::itos(const int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::vector<std::string> util::string::split(const std::string &str, char delimiter)
{
	std::vector<std::string> vec;
	std::stringstream ss(str);
	std::string section;
	while (std::getline(ss, section, delimiter))
	{
		std::stringstream sector(section);
		std::string trimmed;
		sector >> trimmed;
		if (trimmed.size())
			vec.push_back(trimmed);
	}
	return vec;
}

bool util::string::hasSpace(const std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
		if (std::isspace(str[i]))
			return true;
	return false;
};

std::string util::string::toLower(const std::string &str)
{
	std::string ret = str;
	for (size_t i = 0; i < str.length(); i++)
		ret[i] = std::tolower(str[i]);
	return ret;
}

int util::string::hexStrToDecInt(const std::string &hex)
{
	int decimalValue = 0;

	for (std::string::const_iterator it = hex.begin(); it != hex.end(); it++)
	{
		decimalValue *= 16;
		if (std::isdigit(*it))
			decimalValue += (*it - '0');
		else
			decimalValue += (*it - 'a' + 10);
	}
	return (decimalValue);
}

bool util::string::isAllDigit(const std::string &str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!std::isdigit(str[i]))
			return (false);
	}
	return (true);
}
