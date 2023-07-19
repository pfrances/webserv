/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseTools.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 17:53:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 11:51:54 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include <string>
# include <vector>
# include <map>
# include <stdexcept>

class ParseTools {
	public:
		static std::string				getNextToken(std::string const& str,
												std::string::const_iterator& it);
		static std::vector<std::string> getAllTokensUntilSemicolon(std::string const& str,
													std::string::const_iterator& it);
		static std::string				extractBlock(std::string const& conf,
													std::string::const_iterator& it);
		static std::string				intToString(int nb);
		static int						stringToInt(std::string const& str);

		static int						stringIpToInt(std::string const& ip);

		static std::map<std::string,
				std::string>			parseQuery(std::string const& queryStr);
		static bool						isValidToken(std::string const& token,
											std::string const& forbiddenCharset);
		static void						decodeUrlEncodedStr(std::string& str);
		static int						hexaStrToInt(std::string const& hexa);
		static std::string				parseBoundaryBody(std::string const& body,
											std::string const& boundary);
		static std::string				parseUrlEncodedBody(std::string const& body);
};

class ConfigurationException : public std::runtime_error {
	public:
		ConfigurationException(std::string const& msg) : std::runtime_error(msg) {}
};

class ConversionException : public std::runtime_error {
	public:
		ConversionException(std::string const& msg) : std::runtime_error(msg) {}
};

class UnvalidTokenException : public std::runtime_error {
	public:
		UnvalidTokenException(std::string const& msg) : std::runtime_error(msg) {}
};

#endif
