#include "Http/Request.hpp"
#include "util/Parse.hpp"
#include "util/string.hpp"
#include <iostream>

using namespace Hafserv;

Request::Request()
	: parseStatus(Created), connectionClose(false), contentLength(-1), bodyLength(0), body(), chunkSize(0), buffer(),
	  isEnd(false)
{
	oss.str("");
	oss.clear();
}

Request::Request(const Request &other)
	: parseStatus(other.parseStatus), method(other.method), requestTarget(other.requestTarget), headers(other.headers),
	  boundary(other.boundary), connectionClose(other.connectionClose), contentLength(other.contentLength),
	  bodyLength(other.bodyLength), body(other.body), parseBody(other.parseBody), chunkSize(other.chunkSize),
	  buffer(other.buffer)
{
	oss.str("");
	oss.clear();
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		parseStatus = rhs.parseStatus;
		method = rhs.method;
		requestTarget = rhs.requestTarget;
		headers = rhs.headers;
		boundary = rhs.boundary;
		contentLength = rhs.contentLength;
		bodyLength = rhs.bodyLength;
		body = rhs.body;
		parseBody = rhs.parseBody;
		buffer = rhs.buffer;
		oss.str("");
		oss.clear();
		chunkSize = rhs.chunkSize;
		connectionClose = rhs.connectionClose;
	}
	return *this;
}
Request::~Request() {}

int Request::readRequest(const int fd)
{
	unsigned long idx;
	int readBytes = read(fd, charBuf, BUFFER_SIZE);

	if (readBytes <= 0)
	{
		parseStatus = End;
		return -1;
	}
	buffer += std::string(charBuf, readBytes);

	if (parseStatus != Body)
	{
		while ((idx = buffer.find('\n')) != (unsigned long)std::string::npos && parseStatus != Body)
		{
			std::string line = buffer.substr(0, idx + 1);
			int status;

			buffer = buffer.substr(idx + 1);
			if ((status = parse(line)))
				return status;
		}
		if (parseStatus == Body)
			return (this->*parseBody)();
	}
	else
	{
		return (this->*parseBody)();
	}
	return 0;
}

int Request::parse(std::string &request)
{
	if (parseStatus == Created)
		return parseStartLine(request);
	else
		return parseHeaders(request);
	return 0;
}

int Request::parseStartLine(const std::string &request)
{
	std::istringstream requestStream(request);
	std::string rTarget, httpVersion;

	// GET /index.html HTTP/1.1
	std::getline(requestStream, method, ' ');
	std::getline(requestStream, rTarget, ' ');
	requestTarget = RequestTarget(rTarget);
	std::getline(requestStream, httpVersion, '\r');

	if (httpVersion.find("HTTP/") != 0)
	{
		parseStatus = End;
		return 400;
	} // 400 Bad Request
	else
	{
		std::istringstream iss(httpVersion.substr(5));
		double ver;

		iss >> ver;
		if (iss.fail() || !iss.eof())
		{
			parseStatus = End;
			return 400;
		} // 400 Bad Request

		if (httpVersion.substr(5) != "1.1")
		{
			parseStatus = End;
			return 505;
		} // 505 HTTP version not supported
	}
	parseStatus = Header;
	return 0;
}

int Request::parseHeaders(const std::string &fieldLine)
{
	if (fieldLine == "\r\n")
	{
		if (method == "POST")
		{
			if (parseStatus == Header)
			{
				isEnd = false;
				parseStatus = Body;
				setBodyParseFunction();
			}
			else if (parseStatus == Trailer)
				parseStatus = End;
		}
		else
			parseStatus = End;
		return (checkHeaderField());
	}
	std::istringstream iss(fieldLine);
	std::string key, value;

	std::getline(iss, key, ':');
	if (util::string::hasSpace(key))
	{
		parseStatus = End;
		return 400;
	} // 400 Bad Request
	std::getline(iss >> std::ws, value, '\r');
	if (value.length() > HEADER_LIMIT)
	{
		parseStatus = End;
		return 431;
	}
	key = util::string::toLower(key);
	std::map<std::string, std::string>::iterator it = headers.find(key);
	if (it != headers.end())
	{
		it->second += ", ";
		it->second += value;
	}
	else
	{
		headers.insert(make_pair(key, value));
	}
	return 0;
}

void Request::setBodyParseFunction()
{
	// contentLength
	std::map<std::string, std::string>::iterator contentLengthIt = headers.find("content-length");
	if (contentLengthIt != headers.end())
	{
		contentLength = util::string::stoi(contentLengthIt->second).first;
		parseBody = &Request::parseByContentLength;
	}

	// Boundary
	std::map<std::string, std::string>::iterator contentTypeIt = headers.find("content-type");
	if (contentTypeIt != headers.end())
	{
		std::vector<std::string> contentType = parseContentType(contentTypeIt->second);
		std::vector<std::string>::iterator it = std::find(contentType.begin(), contentType.end(), "boundary");
		if (it != contentType.end())
		{
			boundary = *(it + 1);
			parseBody = &Request::parseByBoundary;
		}
	}

	// Transfer-Encoding
	std::map<std::string, std::string>::iterator transferEncodingIt = headers.find("transfer-encoding");
	if (transferEncodingIt != headers.end())
	{
		parseBody = &Request::parseByTransferEncoding;
	}
}

int Request::checkHeaderField()
{
	std::map<std::string, std::string>::iterator transferEncodingIt = headers.find("transfer-encoding");
	std::map<std::string, std::string>::iterator contentLengthIt = headers.find("content-length");
	if (transferEncodingIt != headers.end())
	{
		std::vector<std::string> transferEncoding = parseTransferEncoding(transferEncodingIt->second);
		std::vector<std::string>::iterator chunkedIt =
			std::find(transferEncoding.begin(), transferEncoding.end(), "chunked");
		if (chunkedIt == transferEncoding.end() || chunkedIt + 1 != transferEncoding.end() ||
			contentLengthIt != headers.end())
		{
			parseStatus = End;
			return 400;
		}
	}

	if (method == "POST" && transferEncodingIt == headers.end() && contentLengthIt == headers.end())
	{
		parseStatus = End;
		return 411;
	}

	if (contentLengthIt != headers.end() && !util::string::isAllDigit(contentLengthIt->second))
	{
		parseStatus = End;
		return 400;
	}

	std::map<std::string, std::string>::iterator hostIt = headers.find("host");
	if (hostIt == headers.end() || hostIt->second.find(",") != std::string::npos)
	{
		parseStatus = End;
		return 400;
	}

	std::map<std::string, std::string>::iterator connectionIt = headers.find("connection");
	if (connectionIt != headers.end() && connectionIt->second == "close")
	{
		connectionClose = true;
	}

	return 0;
}

int Request::parseByBoundary()
{
	if (bodyLength + buffer.length() > contentLength)
	{
		oss.write(buffer.c_str(), contentLength - bodyLength);
		buffer.clear();
		bodyLength += (contentLength - bodyLength);
	}
	else
	{
		oss << buffer;
		bodyLength += buffer.length();
		buffer.clear();
	}

	if (bodyLength == contentLength)
	{
		body = oss.str();
		parseStatus = End;
	}
	return 0;
}

int Request::parseByTransferEncoding()
{
	while (true)
	{
		if (chunkSize) // no buffer
		{
			if (chunkSize < buffer.length())
			{
				oss << buffer.substr(0, chunkSize);
				buffer = buffer.substr(chunkSize);
				chunkSize = 0;
			}
			else
			{
				oss << buffer;
				chunkSize -= buffer.length();
				buffer.clear();
				break;
			}
		}
		// read chunksize
		if (isEnd)
		{
			if (headers.find("trailer") == headers.end())
			{
				if (buffer.length() == 2 && buffer.substr(0, 2) == "\r\n")
				{
					body = oss.str();
					parseStatus = End;
					break;
				}
			}
			else
			{
				body = oss.str();
				parseStatus = Trailer;
				break;
			}
		}
		if (!isEnd && buffer.length() >= 2 && buffer.substr(0, 2) == "\r\n")
		{
			buffer.erase(0, 2);
		}
		unsigned long idx;
		if ((idx = buffer.find("\r\n")) != (unsigned long)std::string::npos)
		{
			chunkSize = util::string::hexStrToDecInt(readHex(buffer));
			bodyLength += chunkSize;
			buffer.erase(0, idx + 2);
			if (chunkSize == 0)
			{
				isEnd = true;
				removeChunkField();
			}
		}
		else
			break;
	}

	return 0;
}

int Request::parseByContentLength()
{
	if (bodyLength + buffer.length() > contentLength)
	{
		oss.write(buffer.c_str(), contentLength - bodyLength);
		buffer.clear();
		bodyLength += (contentLength - bodyLength);
	}
	else
	{
		oss << buffer;
		bodyLength += buffer.length();
		buffer.clear();
	}

	if (bodyLength == contentLength)
	{
		body = oss.str();
		parseStatus = End;
	}
	return 0;
}

void Request::removeChunkField()
{
	std::string te = headers.find("transfer-encoding")->second;
	std::vector<std::string> teVec = parseTransferEncoding(te);
	std::ostringstream oss;

	for (std::vector<std::string>::iterator it = teVec.begin(); it < teVec.end(); it++)
	{
		if (*it == "chunked")
			continue;
		if (it + 2 == teVec.end())
			oss << *it;
		else
			oss << *it << ",";
	}
	headers.find("transfer-encoding")->second = oss.str();
}

int Request::getBodyLength() const { return bodyLength; }

int Request::getConnectionClose() const { return connectionClose; }

int Request::getParseStatus() const { return parseStatus; }

const RequestTarget &Request::getRequestTarget() const { return requestTarget; }

const HeaderMultiMap &Request::getHeaders() const { return headers; }

const std::string &Hafserv::Request::getMethod() const { return method; }

const std::string &Hafserv::Request::getBody() const { return body; }

size_t Hafserv::Request::getContentLength() const { return contentLength; }

std::ostream &Hafserv::operator<<(std::ostream &os, const Hafserv::Request &request)
{
	os << request.getMethod() << " " << request.getRequestTarget() << " HTTP/1.1\r\n";
	for (std::map<std::string, std::string>::const_iterator it = request.getHeaders().begin();
		 it != request.getHeaders().end(); it++)
		os << it->first << ": " << it->second << "\r\n";
	os << "bodylen: " << request.getBody().length() << std::endl;
	if (request.getBody().length() < 10000)
		os << "body : " << request.getBody();
	return os;
}
