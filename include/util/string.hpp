/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jukim2 <jukim2@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:43:53 by gyoon             #+#    #+#             */
/*   Updated: 2024/02/18 16:11:29 by jukim2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTIL_STRING_HPP
#define UTIL_STRING_HPP

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Hafserv
{
namespace util
{
namespace string
{
std::pair<int, bool> stoi(const std::string &str);

std::pair<unsigned short, bool> stous(const std::string &str);

std::string itos(const int n);

std::vector<std::string> split(const std::string &str, char delimiter);

bool hasSpace(const std::string &str);

std::string toLower(const std::string &str);

int hexStrToDecInt(const std::string &hex);

bool isAllDigit(const std::string &str);

} // namespace string
} // namespace util
} // namespace Hafserv

#endif
