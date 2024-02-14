#include "Http/Request.hpp"
#include "util/Parse.hpp"
#include "util/string.hpp"
#include <iostream>

using namespace Hafserv;

Request::Request() : parseStatus(Created), contentLength(0), bodyLength(0), isEnd(false) {}

Request::Request(const Request &other)
	: parseStatus(other.parseStatus), method(other.method), requestTarget(other.requestTarget), headers(other.headers),
	  boundary(other.boundary), contentLength(other.contentLength), bodyLength(other.bodyLength), body(other.body),
	  bodyVec(other.bodyVec), parseBody(other.parseBody)
{
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
		bodyVec = rhs.bodyVec;
		parseBody = rhs.parseBody;
	}
	return *this;
}
Request::~Request() {}

int Request::readRequest(const int fd)
{
	int idx;

	if (parseStatus != Body)
	{
		int str_len = recv(fd, charBuf, BUFFER_SIZE, 0);

		buffer += std::string(charBuf, str_len);
		while ((idx = buffer.find('\n')) != std::string::npos && parseStatus != Body)
		{
			std::string line = buffer.substr(0, idx + 1);
			int status;

			buffer = buffer.substr(idx + 1);
			if ((status = parse(line)))
				return status;
		}
	}
	else
	{
		return (this->*parseBody)(fd);
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
				parseStatus = Body;
			else if (parseStatus == Trailer)
				parseStatus = End;
		}
		else
			parseStatus = End;
		return checkHeaderField();
	}
	std::istringstream iss(fieldLine);
	std::string key, value;

	std::getline(iss, key, ':');
	if (util::string::hasSpace(key))
	{
		parseStatus = End;
		return 400;
	} // 400 Bad Request
	std::getline(iss >> std::ws, value);
	key = util::string::toLower(key);
	headers.insert(std::make_pair(key, value));
	return 0;
}

int Request::checkHeaderField()
{
	// contentLength
	std::multimap<std::string, std::string>::iterator contentLengthIt = headers.find("content-length");
	if (contentLengthIt != headers.end())
	{
		contentLength = stoi(contentLengthIt->second);
		parseBody = &Request::parseByContentLength;
	}

	// Boundary
	std::multimap<std::string, std::string>::iterator contentTypeIt = headers.find("content-type");
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
	std::multimap<std::string, std::string>::iterator tranferEncodingIt = headers.find("transfer-encoding");
	if (tranferEncodingIt != headers.end())
	{
		std::vector<std::string> transferEncoding = parseTransferEncoding(tranferEncodingIt->second);
		std::vector<std::string>::iterator it = std::find(transferEncoding.begin(), transferEncoding.end(), "chunked");
		if (it + 1 == transferEncoding.end())
			parseBody = &Request::parseByTransferEncoding;
		if (it == transferEncoding.end() || contentLengthIt != headers.end())
		{
			parseStatus = End;
			return 400;
		}
	}
	return 0;
}

int Request::parseByBoundary(const int &fd)
{
	ssize_t bytesRead = read(fd, charBuf, BUFFER_SIZE);

	if (bodyLength + bytesRead > contentLength)
	{
		buffer += std::string(charBuf, contentLength - bodyLength);
		bodyLength += (contentLength - bodyLength);
	}
	else
	{
		oss.write(charBuf, bytesRead);
		bodyLength += bytesRead;
	}

	if (bodyLength == contentLength)
	{
		body = oss.str();
		parseStatus = End;
	}
	return 0;
}

int Request::parseByTransferEncoding(const int &fd)
{
	static int chunkSize = 0;
	ssize_t bytesRead = read(fd, charBuf, BUFFER_SIZE);
	buffer += std::string(charBuf, bytesRead);

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
		if (isEnd) // length말고 첫 두글자
		{
			if (headers.find("trailer") == headers.end())
			{
				if (buffer.substr(0, 2) == "\r\n")
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
			buffer.erase(0, 2);
		int idx;
		if ((idx = buffer.find("\r\n")) != std::string::npos)
		{
			chunkSize = util::string::hexStrToDecInt(readHex(buffer));
			buffer.erase(0, idx + 2);
			if (chunkSize == 0)
			{
				isEnd = true;
				removeChunkField("");
			}
		}
		else
			break;
	}

	return 0;
}

int Request::parseByContentLength(const int &fd)
{
	ssize_t bytesRead = read(fd, charBuf, BUFFER_SIZE);

	if (bodyLength + bytesRead > contentLength)
	{
		buffer += std::string(charBuf, contentLength - bodyLength);
		bodyLength += (contentLength - bodyLength);
	}
	else
	{
		oss.write(charBuf, bytesRead);
		bodyLength += bytesRead;
	}

	if (bodyLength == contentLength)
	{
		body = oss.str();
		parseStatus = End;
	}
	return 0;
}

void Request::removeChunkField(const std::string &fieldName)
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

std::string Request::getRawRequest()
{
	std::stringstream ss;

	ss << method << " " << requestTarget << " HTTP/1.1\r\n";
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		ss << it->first << ": " << it->second << "\r\n";
	if (method == "POST")
		ss << "\r\n" << body << "\r\n";
	return ss.str();
}

void Request::printRequest() const
{
	std::cout << "<-------request------->" << std::endl;
	std::cout << method << " " << requestTarget << " HTTP/1.1\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
		std::cout << it->first << ": " << it->second << "\r\n";
	// std::cout << body;
	std::cout << "<-----request end----->" << std::endl;
}

const int Hafserv::Request::getParseStatus() const { return parseStatus; }

const RequestTarget &Request::getRequestTarget() const { return requestTarget; }

const HeaderMultiMap &Request::getHeaders() const { return headers; }

const std::string &Hafserv::Request::getMethod() const { return method; }

const std::string &Hafserv::Request::getBody() const { return body; }

const size_t Hafserv::Request::getContentLength() const { return contentLength; }
