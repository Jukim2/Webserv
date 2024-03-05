#include "util/Parse.hpp"
#include "util/string.hpp"

std::vector<std::string> parseTransferEncoding(std::string &str)
{
	std::vector<std::string> vec;
	std::string::iterator it = str.begin();

	while (it != str.end())
	{
		if (!readTransferCoding(vec, str, it))
		{
			vec.clear();
			return (vec);
		}
	}
	return (vec);
}

bool readTransferCoding(std::vector<std::string> &vec, std::string &str, std::string::iterator &it)
{
	if (readOWS(it) && readToken(vec, str, it))
	{
		while (it != str.end() && *(it++) != ',')
			;
		return (true);
	}
	return (false);
}

std::vector<std::string> parseContentType(std::string &str)
{
	std::vector<std::string> vec;
	std::string::iterator it = str.begin();

	if (readToken(vec, str, it) && (*it++ == '/') && readToken(vec, str, it) && readParameters(vec, str, it))
	{
		return (vec);
	}
	else
	{
		vec.clear();
		return (vec);
	}
}

bool readTokenOrQuote(std::vector<std::string> &vec, std::string &str, std::string::iterator &it)
{
	if (*it == '\"')
	{
		return (readQuote(vec, ++it));
	}
	else
		return (readToken(vec, str, it));
}

bool readToken(std::vector<std::string> &vec, std::string &str, std::string::iterator &it)
{
	std::string::iterator initIt = it;
	while (isTokenChar(*it))
		it++;
	vec.push_back(str.substr(initIt - str.begin(), it - initIt));
	return (initIt != it);
}

bool isTokenChar(const char &ch)
{
	if (std::isalnum(ch) || ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' || ch == '*' ||
		ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' || ch == '`' || ch == '|' || ch == '~')
		return (true);
	else
		return (false);
}

bool readQuote(std::vector<std::string> &vec, std::string::iterator &it)
{
	std::string quotedStr;

	while (isQdtext(*it) || (*it == '\\' && isQuotedPair(*(++it))))
	{
		quotedStr += *it;
		it++;
	}
	vec.push_back(quotedStr);
	return (*(++it) == '\"');
}

bool isQdtext(const unsigned char &ch)
{
	if (ch == HTAB || ch == SP || ch == '!' || (35 <= ch && ch <= 91) || (93 <= ch && ch <= 126) || ch > 127)
		return (true);
	else
		return (false);
}

bool isQuotedPair(const unsigned char &ch)
{
	if (ch == HTAB || ch == SP || (33 <= ch && ch <= 126) || ch > 127)
		return (true);
	else
		return (false);
}

bool readParameters(std::vector<std::string> &vec, std::string &str, std::string::iterator &it)
{
	std::string name, value;

	while (str.end() != it)
	{
		if (!(readOWS(it) && *(it++) == ';' && readOWS(it) && readParameter(vec, str, it)))
			return (false);
	}
	return (true);
}

bool readParameter(std::vector<std::string> &vec, std::string &str, std::string::iterator &it)
{
	readToken(vec, str, it);
	if (*it++ != '=')
		return (false);
	return (readTokenOrQuote(vec, str, it));
}

bool readOWS(std::string::iterator &it)
{
	while (*it == SP || *it == HTAB)
		it++;
	return (true);
}

bool readRWS(std::string::iterator &it)
{
	std::string::iterator initIt = it;

	while (*it == SP || *it == HTAB)
		it++;
	return (initIt != it);
}

std::string readHex(std::string &str)
{
	std::string::iterator it;
	for (it = str.begin(); it != str.end(); it++)
	{
		if (!std::isxdigit(*it))
			break;
	}
	return Hafserv::util::string::toLower(str.substr(0, it - str.begin()));
}

void printVector(std::vector<std::string> &vec)
{
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		std::cout << "|" << *it << "|   ";
	}
}
