#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <stdexcept>
#include <string>

class NoBraceError : public std::logic_error
{
public:
	NoBraceError(const std::string &directive) : std::logic_error("directive \"" + directive + "\" has no opening {"){};
	virtual ~NoBraceError() throw(){};
};

class NoSemicolonError : public std::logic_error
{
public:
	NoSemicolonError(const std::string &directive)
		: std::logic_error("directive \"" + directive + "\"  is not terminated by \";\""){};
	virtual ~NoSemicolonError() throw(){};
};

class UnknownDirectiveError : public std::logic_error
{
public:
	UnknownDirectiveError(const std::string &directive) : std::logic_error("unknown directive \"" + directive + "\""){};
	virtual ~UnknownDirectiveError() throw(){};
};

class DuplicateDirectiveError : public std::logic_error
{
public:
	DuplicateDirectiveError(const std::string &directive) : std::logic_error("\"" + directive + "\" is duplicated"){};
	DuplicateDirectiveError(const std::string &d1, const std::string &d2)
		: std::logic_error("\"" + d1 + "\" is duplicated, \"" + d2 + "\" directive was specified earlier"){};
	virtual ~DuplicateDirectiveError() throw(){};
};

class DuplicateListenError : public std::logic_error
{
public:
	DuplicateListenError(const std::string &port) : std::logic_error("a duplicate listen 0.0.0.0:" + port){};
	virtual ~DuplicateListenError() throw(){};
};

class DuplicateLocationError : public std::logic_error
{
public:
	DuplicateLocationError(const std::string &pattern) : std::logic_error("duplicate location \"" + pattern + "\""){};
	virtual ~DuplicateLocationError() throw(){};
};

class DisallowDirectiveError : public std::logic_error
{
public:
	DisallowDirectiveError(const std::string &directive)
		: std::logic_error("\"" + directive + "\" directive is not allowed here"){};
	virtual ~DisallowDirectiveError() throw(){};
};

class InvalidNumberArgumentError : public std::logic_error
{
public:
	InvalidNumberArgumentError(const std::string &directive)
		: std::logic_error("invalid number of arguments in \"" + directive + "\" directive"){};
	virtual ~InvalidNumberArgumentError() throw(){};
};

class InvalidErrorCodeError : public std::logic_error
{
public:
	InvalidErrorCodeError(const std::string &value)
		: std::logic_error("value \"" + value + "\"must be between 300 and 599"){};
	virtual ~InvalidErrorCodeError() throw(){};
};

class InvalidArgumentError : public std::logic_error
{
public:
	InvalidArgumentError(const std::string &directive)
		: std::logic_error("invalid argument in \"" + directive + "\" directive"){};
	InvalidArgumentError(const std::string &d1, const std::string &d2)
		: std::logic_error("invalid argument in \"" + d1 + "\" directive: \"" + d2 + "\""){};
	virtual ~InvalidArgumentError() throw(){};
};

class ParseError : public std::logic_error
{
public:
	ParseError(const std::string &msg) : std::logic_error(msg){};
	virtual ~ParseError() throw(){};
};

class IncludeError : public std::logic_error
{
public:
	IncludeError(const std::string &msg) : std::logic_error(msg){};
	virtual ~IncludeError() throw(){};
};

#endif