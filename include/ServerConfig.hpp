/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyoon <gyoon@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 13:46:10 by gyoon             #+#    #+#             */
/*   Updated: 2024/02/09 14:48:45 by gyoon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "AHttpConfigModule.hpp"
#include "ConfigFile.hpp"
#include "LocationConfig.hpp"
#include <iostream>
#include <string>
#include <vector>

#include <sstream>

namespace Hafserv
{
class ServerConfig : public AHttpConfigModule
{
public:
	ServerConfig();
	ServerConfig(const ServerConfig &other);
	ServerConfig(const ConfigFile &block, const HttpConfigCore &core);
	ServerConfig &operator=(const ServerConfig &other);
	~ServerConfig();

	const std::vector<std::string> &getNames() const;
	const std::vector<unsigned short> &getPorts() const;
	const std::vector<std::vector<LocationConfig> > &getLocations() const;

private:
	std::vector<std::string> names;
	std::vector<unsigned short> ports;
	std::vector<std::vector<LocationConfig> > locations;
};

} // namespace Hafserv

std::ostream &operator<<(std::ostream &os, const Hafserv::ServerConfig &conf);

#endif
