/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyoon <gyoon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 22:46:19 by gyoon             #+#    #+#             */
/*   Updated: 2024/02/11 21:17:17 by gyoon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVCONFIG_HPP
#define WEBSERVCONFIG_HPP

#include "Config/AConfig.hpp"
#include "Config/ConfigException.hpp"
#include "Config/EventConfig.hpp"
#include "Config/HttpConfig.hpp"
#include "File/ConfigFile.hpp"
#include <string>

namespace Hafserv
{
class WebservConfig : public AConfig
{
public:
	WebservConfig();
	WebservConfig(const WebservConfig &other);
	WebservConfig(const ConfigFile &configFile) throw(ParseError);
	WebservConfig &operator=(const WebservConfig &other);
	~WebservConfig();

	const ConfigFile::directives_t &getDirectives() const;
	const HttpConfig &getHttpConfig() const;
	const EventConfig &getEventConfig() const;

private:
	ConfigFile::directives_t directives;
	EventConfig event;
	HttpConfig http;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::WebservConfig &conf);

#endif