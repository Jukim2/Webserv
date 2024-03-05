#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Http/RequestTarget.hpp"
#include "Server.hpp"
#include <ctime>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <vector>

#define BUFFER_SIZE 100000
#define HEADER_LIMIT 200

namespace Hafserv
{

typedef std::map<std::string, std::string> HeaderMultiMap;

enum RequestParseStatus
{
	Created,
	StartLine,
	Header,
	Body,
	Trailer,
	End
};

class Request
{
public:
	Request();
	Request(const Request &other);
	Request &operator=(const Request &rhs);
	~Request();
	int readRequest(const int fd);
	int parse(std::string &request);
	int parseStartLine(const std::string &request);
	int parseHeaders(const std::string &fieldLine);
	int parseByContentLength();
	int checkHeaderField();
	int parseByBoundary();
	int parseByTransferEncoding();
	void setBodyParseFunction();
	void removeChunkField();

	int getBodyLength() const;
	int getConnectionClose() const;
	int getParseStatus() const;
	const RequestTarget &getRequestTarget() const;
	const HeaderMultiMap &getHeaders() const;
	const std::string &getMethod() const;
	const std::string &getBody() const;
	size_t getContentLength() const;

	typedef int (Request::*ParseBodyFunction)();

private:
	RequestParseStatus parseStatus;

	std::string method;
	RequestTarget requestTarget;
	HeaderMultiMap headers;
	std::string boundary;
	bool connectionClose;
	size_t contentLength;
	size_t bodyLength;
	std::string body;
	std::ostringstream bodyStream;
	ParseBodyFunction parseBody;
	size_t chunkSize;

	std::string buffer;
	std::ostringstream oss;
	char charBuf[BUFFER_SIZE + 1];
	bool isEnd;
};

std::ostream &operator<<(std::ostream &os, const Hafserv::Request &request);

} // namespace Hafserv

/*
	스타트라인 파싱

	해드 필드 파싱
	1. :으로 구분 필드 네임과 필드 값 분리
	2. 필드 네임 검사
		E : 400코드 리턴
	3. 필드 네임에 따라 파싱함수 적용
		E :
	4. 반복
*/

#endif
