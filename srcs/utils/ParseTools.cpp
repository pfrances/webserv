/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseTools.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 17:58:48 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 11:54:28 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseTools.hpp"
#include <stdexcept>
#include <sstream>
#include "Request.hpp"
#include <iostream>

std::string ParseTools::extractBlock(std::string const& conf, std::string::const_iterator& it) {
	std::string::const_iterator	ite = conf.end();

	while (std::isspace(*it)) {
		it++;
	}
	if (*it != '{') {
		throw std::runtime_error("ParseTools::extractBlock: first char is not '{'.");
	}
	int	bracketCount = 1;
	it++;
	std::string::const_iterator	blockStart = it;
	while (it != ite)
	{
		if (*it == '{') {
			bracketCount++;
		} else if (*it == '}') {
			if (bracketCount == 0) {
				throw std::runtime_error("ParseTools::extractBlock: wrong bracket count.");
			}
			bracketCount--;
		}
		if (bracketCount == 0) {
			it++;
			return std::string(blockStart, it - 1);
		}
		it++;
	}
	if (bracketCount != 0)
		throw std::runtime_error("ParseTools::extractBlock: bracketCount is not 0.");
	return std::string();
}

std::vector<std::string> ParseTools::getAllTokensUntilSemicolon(std::string const& str,
													std::string::const_iterator& it) {
	std::vector<std::string>	tokens;

	std::string					token = getNextToken(str, it);
	while (token != ";" && token.empty() == false) {
		tokens.push_back(token);
		token = getNextToken(str, it);
	}
	if (token != ";") {
		throw std::runtime_error("ParseTools::getAllTokensUntilSemicolon: last token is not ';'.");
	}
	return tokens;
}

std::string	ParseTools::getNextToken(std::string const& str,
											std::string::const_iterator& it) {

	std::string::const_iterator	ite = str.end();
	while (it != ite)
	{
		if (std::isspace(*it)) {
			it++;
		} else {
			std::string::const_iterator	tokenStart = it;
			switch (*it) {
				case '{':
				case '}':
				case ';':
					it++;
					return std::string(tokenStart, it);
				case '#':
					while (it != ite && *it != '\n') {
						it++;
					}
					break;
				default:
					while (it != ite && *it != ' ' && *it != '\t' && *it != '\n'
							&& *it != '{' && *it != ';' && *it != '#') {
						it++;
					}
					return std::string(tokenStart, it);
			}
		}
	}
	return std::string();
}

std::string	ParseTools::intToString(int nb) {
	std::stringstream	ss;

	ss << nb;
	return ss.str();
}

int	ParseTools::stringToInt(std::string const& str) {
	std::istringstream iss(str);
	int value = 0;
	if (!str.empty() && !(iss >> value)) {
		throw std::runtime_error("Failed to convert string to int");
	}
	return value;
}

int		ParseTools::hexaStrToInt(std::string const& hexa) {
	std::istringstream iss(hexa);
	int value = 0;
	if (!(iss >> std::hex >> value)) {
		throw std::runtime_error("Failed to convert hexa string to int");
	}
	return value;
}

void	ParseTools::decodeUrlEncodedStr(std::string& str) {
	std::string::iterator it = str.begin();
	std::string::iterator ite = str.end();

	std::string decodedStr;
	decodedStr.reserve(str.length());
	for (; it != ite; it++) {
		if (*it == '%') {
			if (it + 2 >= ite) {
				throw std::runtime_error("ParseTools::decodeUrlEncodedStr: invalid url encoded string.");
			}
			std::string hexStr(it + 1, it + 3);
			int hex = hexaStrToInt(hexStr);
			decodedStr += static_cast<char>(hex);
			it += 2;
		} else {
			decodedStr += *it;
		}
	}
	str = decodedStr;
}

std::map<std::string, std::string>	ParseTools::parseQuery(std::string const& queryStr) {
	std::string::const_iterator it = queryStr.begin();
	std::string::const_iterator ite = queryStr.end();

	std::map<std::string, std::string> queryMap;

	std::string key;
	std::string token;
	for (; it != ite; it++) {
		if (*it == '=') {
			key = token;
			token = "";
		} else if (*it == '&') {
			decodeUrlEncodedStr(key);
			decodeUrlEncodedStr(token);
			queryMap.insert(std::pair<std::string, std::string>(key, token));
			key = "";
			token = "";
		} else {
			token += *it;
		}
	}
	if (key.length() > 0) {
		decodeUrlEncodedStr(key);
		decodeUrlEncodedStr(token);
		queryMap.insert(std::pair<std::string, std::string>(key, token));
	}
	return queryMap;
}

bool	ParseTools::isValidToken(std::string const& token, std::string const& forbiddenCharset) {
	if (token.find_first_of(forbiddenCharset) != std::string::npos) {
		return false;
	}
	return true;
}

std::string ParseTools::parseBoundaryBody(std::string const& body, std::string const& boundary) {
	std::string delimiter = "--" + boundary + "\r\n";
	size_t start_index = body.find(delimiter);
	if (start_index == std::string::npos) {
		throw ConversionException("ParseTools::parseBoundaryBody: invalid boundary.");
	}
	start_index = body.find("\r\n\r\n", start_index);
	if (start_index == std::string::npos) {
		throw ConversionException("ParseTools::parseBoundaryBody: invalid content format.");
	}
	start_index += 4;

	size_t end_index = body.find("\r\n--" + boundary + "--\r\n", start_index);
	std::string content = body.substr(start_index, end_index - start_index);

	return content;
}

std::string	ParseTools::parseUrlEncodedBody(std::string const& body) {
	std::string::const_iterator it = body.begin();
	std::string::const_iterator ite = body.end();

	std::string decodedStr;
	std::string hexStr;
	decodedStr.reserve(body.length());
	for (; it != ite; it++) {
		if (*it == '+') {
			decodedStr += ' ';
		} else if (*it == '%') {
			if (it + 2 >= ite) {
				hexStr = std::string(it + 1, ite);
				it = ite;
			} else {
				hexStr = std::string(it + 1, it + 3);
				it += 2;
			}
			int hex = hexaStrToInt(hexStr);
			decodedStr += static_cast<char>(hex);
		} else {
			decodedStr += *it;
		}
	}
	return decodedStr;
}

int	ParseTools::stringIpToInt(std::string const& ip) {
	std::string::const_iterator it = ip.begin();
	std::string::const_iterator ite = ip.end();
	int result = 0;
	int bit = 0;
	for (; it != ite; it++) {
		if (*it == '.') {
			result = (result << 8) | bit;
			bit = 0;
		} else {
			bit = bit * 10 + (*it - '0');
		}
	}
	result = (result << 8) | bit;
	return result;
}
