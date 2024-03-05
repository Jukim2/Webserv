#include "Http/Response.hpp"
#include "File/File.hpp"
#include "Webserv.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace Hafserv;

Response::Response() : responseState(Created), writtenBytes(0), responseBytes(0) {}

Response::Response(const Response &other)
	: responseState(other.responseState), statusLine(other.statusLine), headers(other.headers), body(other.body),
	  writtenBytes(0), responseBytes(0)
{
}

Response &Response::operator=(const Response &rhs)
{
	if (this != &rhs)
	{
		responseState = rhs.responseState;
		responseBuffer = rhs.responseBuffer;
		writtenBytes = rhs.writtenBytes;
		responseBytes = rhs.responseBytes;
		statusLine = rhs.statusLine;
		headers = rhs.headers;
		body = rhs.body;
	}
	return *this;
}

Response::~Response() {}

void Response::makeBody(const LocationConfig &targetLocationConfig, const std::string &targetResource)
{
	// targetResource = error/413.html
	const std::multimap<std::string, std::string> &typeMap = targetLocationConfig.getTypes();
	std::multimap<std::string, std::string>::const_iterator typeIt =
		typeMap.find(targetResource.substr(targetResource.rfind('.') + 1));
	if (typeIt != typeMap.end())
	{
		std::string contentType = typeIt->second;
		if (typeIt->second == "text/html" || typeIt->second == "text/css" || typeIt->second == "text/xml")
			contentType += "; charset=UTF-8";
		addToHeaders("Content-Type", contentType);
	}
	else
		addToHeaders("Content-Type", "application/octet-stream");
	RegularFile targetFile(targetResource);
	body = targetFile.getContents();

	std::ostringstream contentLengthOss;
	contentLengthOss << body.length();
	addToHeaders("Content-Length", contentLengthOss.str());
}

void Response::makeErrorBody(int statusCode)
{
	addToHeaders("Content-Type", "text/html");
	body = "<html>"
		   "<head><title>" +
		   Webserv::getInstance().getStatusMessage(statusCode) +
		   "</title></head>"
		   "<body>"
		   "<center><h1>" +
		   Webserv::getInstance().getStatusMessage(statusCode) +
		   "</h1></center>"
		   "<hr><center>Hafserv/1.0.0</center>"
		   "</body>"
		   "</html>";
	std::ostringstream contentLengthOss;
	contentLengthOss << body.length();
	addToHeaders("Content-Length", contentLengthOss.str());
}

std::string Response::generateDate()
{
	std::time_t currentTime = std::time(NULL);
	std::tm *timeInfo = std::localtime(&currentTime);
	char buffer[80];

	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

	return std::string(buffer);
}

void Response::setStatusLine(std::string statusLine) { this->statusLine = statusLine; }

void Response::setBody(const std::string &bodyString)
{
	body = bodyString;
	addToHeaders("Content-Length", util::string::itos(body.length()));
}

Response::ResponseState Response::getResponseState() const { return responseState; }

void Response::setResponseState(ResponseState rs) { responseState = rs; }

void Response::setResponseBuffer()
{
	std::ostringstream oss;

	oss << statusLine << CRLF;
	addToHeaders("Date", generateDate());
	for (HeaderMultiMap::iterator it = headers.begin(); it != headers.end(); it++)
		oss << it->first << ": " << it->second << CRLF;
	oss << CRLF;
	oss << body;
	responseBuffer = oss.str();
	responseBytes = responseBuffer.length();
	writtenBytes = 0;
	responseState = Ready;
}

void Response::addToHeaders(std::string key, std::string value) { headers.insert(std::make_pair(key, value)); }

void Response::removeHeaders(std::string key) { headers.erase(key); }

int Response::getWrittenBytes() const { return writtenBytes; }

int Response::getResponseBytes() const { return responseBytes; }

bool Response::send(int fd)
{
	const char *wrBuffer = responseBuffer.c_str();
	size_t bytesToWrite = std::min(responseBytes - writtenBytes, (unsigned long)BUFFER_SIZE);
	int ret = write(fd, wrBuffer + writtenBytes, bytesToWrite);
	if (ret < 1)
	{
		responseState = End;
		return false;
	}
	else
		writtenBytes += ret;
	if (writtenBytes == responseBytes)
		responseState = End;
	return true;
}
