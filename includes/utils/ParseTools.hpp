/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseTools.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 17:53:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/29 12:35:16 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include <string>
# include <vector>

class Server;

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
};

#endif
