/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 17:53:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/18 19:21:29 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP

# include <string>
# include <vector>

class ServConf;

class ConfParser {
	public:
		ServConf parseSingleServConf(std::string confFileName) const;
		std::vector<ServConf> parseMultiServConf(std::string confFileName) const;
	
};

#endif
