/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseTools.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 17:58:48 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/24 13:10:56 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseTools.hpp"
#include <stdexcept>
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
