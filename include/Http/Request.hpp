/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinhchoi <jinhchoi@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 19:58:18 by jinhchoi          #+#    #+#             */
/*   Updated: 2024/02/09 20:07:15 by jinhchoi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Server.hpp"
#include <ctime>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace Hafserv
{

typedef std::multimap<std::string, std::string> HeaderMultiMap;

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
	// Request(const Request &other);
	// Request &operator=(const Request &rhs);
	// ~Request();
	int parse(std::string request);
	int parseStartLine(const std::string &request);
	int parseHeaders(const std::string &fieldLine);
	int parseByContentLength(std::string &line);
	int parseByBoundary(std::string &line);
	int parseByTransferEncoding(std::string &line);
	std::string getRawRequest();
	void printRequest();
	void printBody();
	void checkHeaderField();

	const int getParseStatus() const;
	const std::string getRequestTarget() const;
	const HeaderMultiMap &getHeaders() const;
	const std::string &getMethod() const;
	const std::string &getBody() const;

	typedef int (Request::*ParseBodyFunction)(std::string &);
	friend class Response;

private:
	RequestParseStatus parseStatus;

	std::string method;
	std::string requestTarget;
	HeaderMultiMap headers;
	std::string boundary;
	size_t contentLength;
	size_t bodyLength;
	std::string body;
	std::vector<std::string> bodyVec;
	ParseBodyFunction parseBody;
};

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