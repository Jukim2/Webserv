/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfigCore.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyoon <gyoon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 16:26:26 by gyoon             #+#    #+#             */
/*   Updated: 2024/01/20 17:13:58 by gyoon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfigCore.hpp"

using namespace Hafserv;

HttpConfigCore::Timeout::Timeout() : clientHeader(60), clientBody(60), keepAlive(75), send(60) {}

HttpConfigCore::HttpConfigCore() : root(), indexes(), timeouts(), errorPages() {}

HttpConfigCore::HttpConfigCore(const HttpConfigCore &other)
	: root(other.root), indexes(other.indexes), timeouts(other.timeouts), errorPages(other.errorPages),
	  types(other.types)
{
}

HttpConfigCore &HttpConfigCore::operator=(const HttpConfigCore &other)
{
	if (this != &other)
	{
		root = other.root;
		indexes = other.indexes;
		timeouts = other.timeouts;
		errorPages = other.errorPages;
		types = other.types;
	}
	return *this;
}

HttpConfigCore::~HttpConfigCore() {}

const std::string &HttpConfigCore::getRoot() const { return root; }

const std::vector<std::string> &HttpConfigCore::getIndexes() const { return indexes; }

const HttpConfigCore::Timeout &HttpConfigCore::getTimeout() const { return timeouts; }

const std::map<std::string, std::string> &HttpConfigCore::getErrorPages() const { return errorPages; }

const std::multimap<std::string, std::string> HttpConfigCore::getTypes() const { return types; }

void HttpConfigCore::setRoot(const std::string &root) { this->root = root; }

void HttpConfigCore::setIndexes(const std::vector<std::string> &indexes) { this->indexes = indexes; }

void HttpConfigCore::setTimeouts(const Timeout &timeouts) { this->timeouts = timeouts; }

void HttpConfigCore::setClientHeaderTimeout(int timeout) { this->timeouts.clientHeader = timeout; }

void HttpConfigCore::setClientBodyTimeout(int timeout) { this->timeouts.clientBody = timeout; }

void HttpConfigCore::setKeepAliveTimeout(int timeout) { this->timeouts.keepAlive = timeout; }

void HttpConfigCore::setSendTimeout(int timeout) { this->timeouts.send = timeout; }

void HttpConfigCore::setErrorPages(const std::map<std::string, std::string> &errorPages)
{
	this->errorPages = errorPages;
}
void HttpConfigCore::setTypes(const std::multimap<std::string, std::string> &types) { this->types = types; }

std::ostream &operator<<(std::ostream &os, const HttpConfigCore &conf)
{
	os << "\t[HttpConfigCore]" << std::endl;
	os << "\t\troot: " << conf.getRoot() << std::endl;
	os << "\t\tindexes: ";
	for (size_t i = 0; i < conf.getIndexes().size(); i++)
		os << conf.getIndexes().at(i) << " ";
	os << std::endl;
	os << conf.getTimeout();
	return os;
}

std::ostream &operator<<(std::ostream &os, const HttpConfigCore::Timeout &timeouts)
{
	os << "\t\tclientHeaderTimeouts: " << timeouts.clientHeader << std::endl;
	os << "\t\tclientBodyTimeouts: " << timeouts.clientBody << std::endl;
	os << "\t\tkeepAliveTimeouts: " << timeouts.keepAlive << std::endl;
	os << "\t\tsendTimeouts: " << timeouts.send;
	return os;
}