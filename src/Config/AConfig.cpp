/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConfig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyoon <gyoon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 17:46:49 by gyoon             #+#    #+#             */
/*   Updated: 2024/02/11 21:57:51 by gyoon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config/AConfig.hpp"

using namespace Hafserv;

std::set<std::string> AConfig::allSimpleDirectives;
std::set<std::string> AConfig::mainSimpleDirectives;
std::set<std::string> AConfig::eventsSimpleDirectives;
std::set<std::string> AConfig::httpSimpleDirectives;
std::set<std::string> AConfig::serverSimpleDirectives;
std::set<std::string> AConfig::locationSimpleDirectives;

std::set<std::string> AConfig::allBlockDirectives;
std::set<std::string> AConfig::mainBlockDirectives;
std::set<std::string> AConfig::eventsBlockDirectives;
std::set<std::string> AConfig::httpBlockDirectives;
std::set<std::string> AConfig::serverBlockDirectives;
std::set<std::string> AConfig::locationBlockDirectives;

AConfig::AConfig()
{
	if (allSimpleDirectives.empty())
		initAllSimpleDirectives();
	if (allBlockDirectives.empty())
		initAllBlockDirectives();
}

AConfig::AConfig(const AConfig &other) {}

AConfig &AConfig::operator=(const AConfig &other)
{
	if (this != &other)
	{
	}
	return *this;
}

AConfig::~AConfig() {}

// void initDirectives() {}

void AConfig::initAllSimpleDirectives()
{
	/*		WebservSimpleDirectives		*/
	allSimpleDirectives.insert("user");
	allSimpleDirectives.insert("worker_processes");
	allSimpleDirectives.insert("error_log");
	allSimpleDirectives.insert("pid");
	allSimpleDirectives.insert("worker_rlimit_nofile");

	/*		EventSimpleDirectives		*/
	allSimpleDirectives.insert("worker_connections");

	/*		HttpSimpleDirectives		*/
	allSimpleDirectives.insert("default_type");
	allSimpleDirectives.insert("sendfile");

	/*		ServerSimpleDirectives		*/
	allSimpleDirectives.insert("listen");
	allSimpleDirectives.insert("server_name");

	/*		LocationSimpleDirectives	*/
	allSimpleDirectives.insert("alias");
	allSimpleDirectives.insert("proxy_pass");
	allSimpleDirectives.insert("cgi_path");

	/*		HttpSimpleDirectives		*/
	allSimpleDirectives.insert("root");
	allSimpleDirectives.insert("index");
	allSimpleDirectives.insert("client_header_timeout");
	allSimpleDirectives.insert("client_body_timeout");
	allSimpleDirectives.insert("keepalive_timeout");
	allSimpleDirectives.insert("send_timeout");
	allSimpleDirectives.insert("error_page");
	allSimpleDirectives.insert("allow_methods");
}

void AConfig::initMainSimpleDirectives()
{
	/*		WebservSimpleDirectives		*/
	allSimpleDirectives.insert("user");
	allSimpleDirectives.insert("worker_processes");
	allSimpleDirectives.insert("error_log");
	allSimpleDirectives.insert("pid");
	allSimpleDirectives.insert("worker_rlimit_nofile");
}

void AConfig::initEventsSimpleDirectives()
{ /*		EventSimpleDirectives		*/
	allSimpleDirectives.insert("worker_connections");
}

void AConfig::initHttpSimpleDirectives()
{
	/*		CoreSimpleDirectives		*/
	httpSimpleDirectives.insert("root");
	httpSimpleDirectives.insert("index");
	httpSimpleDirectives.insert("client_header_timeout");
	httpSimpleDirectives.insert("client_body_timeout");
	httpSimpleDirectives.insert("keepalive_timeout");
	httpSimpleDirectives.insert("send_timeout");
	httpSimpleDirectives.insert("error_page");
	httpSimpleDirectives.insert("allow_methods");

	/*		HttpSimpleDirectives		*/
	httpBlockDirectives.insert("default_type");
	httpBlockDirectives.insert("sendfile");
}

void AConfig::initServerSimpleDirectives()
{
	/*		CoreSimpleDirectives		*/
	serverSimpleDirectives.insert("root");
	serverSimpleDirectives.insert("index");
	serverSimpleDirectives.insert("client_header_timeout");
	serverSimpleDirectives.insert("client_body_timeout");
	serverSimpleDirectives.insert("keepalive_timeout");
	serverSimpleDirectives.insert("send_timeout");
	serverSimpleDirectives.insert("error_page");
	serverSimpleDirectives.insert("allow_methods");

	/*		ServerSimpleDirectives		*/
	httpBlockDirectives.insert("listen");
	httpBlockDirectives.insert("server_name");
}

void AConfig::initLocationSimpleDirectives()
{
	/*		CoreSimpleDirectives		*/
	locationSimpleDirectives.insert("root");
	locationSimpleDirectives.insert("index");
	locationSimpleDirectives.insert("client_body_timeout");
	locationSimpleDirectives.insert("keepalive_timeout");
	locationSimpleDirectives.insert("send_timeout");
	locationSimpleDirectives.insert("error_page");
	locationSimpleDirectives.insert("allow_methods");

	/*		LocationSimpleDirectives		*/
	httpBlockDirectives.insert("alias");
	httpBlockDirectives.insert("proxy_pass");
	httpBlockDirectives.insert("cgi_path");
}

void AConfig::initCoreSimpleDirectives()
{
	/*		CoreSimpleDirectives		*/
	coreSimpleDirectives.insert("root");
	coreSimpleDirectives.insert("index");
	coreSimpleDirectives.insert("client_header_timeout");
	coreSimpleDirectives.insert("client_body_timeout");
	coreSimpleDirectives.insert("keepalive_timeout");
	coreSimpleDirectives.insert("send_timeout");
	coreSimpleDirectives.insert("error_page");
	coreSimpleDirectives.insert("allow_methods");
}

void AConfig::initAllBlockDirectives()
{
	allBlockDirectives.insert("events");
	allBlockDirectives.insert("http");
	allBlockDirectives.insert("server");
	allBlockDirectives.insert("location");
	allBlockDirectives.insert("types");
}

void AConfig::initMainBlockDirectives()
{
	mainBlockDirectives.insert("events");
	mainBlockDirectives.insert("http");
}

void AConfig::initEventsBlockDirectives() {}

void AConfig::initHttpBlockDirectives()
{
	httpBlockDirectives.insert("types");
	httpBlockDirectives.insert("server");
}

void AConfig::initServerBlockDirectives()
{
	serverBlockDirectives.insert("types");
	serverBlockDirectives.insert("location");
}

void AConfig::initLocationBlockDirectives() { locationBlockDirectives.insert("types"); }

void AConfig::initCoreBlockDirectives() { coreBlockDirectives.insert("types"); }