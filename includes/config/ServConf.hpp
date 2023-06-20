/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:10:23 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:20:15 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONF_HPP
# define SERVCONF_HPP

# include <string>
# include <map>
# include <vector>
# include <netinet/in.h>
# include "Location.hpp"

class ServConf {

public:
	ServConf(std::string confFile);
	~ServConf(void);
	ServConf(const ServConf &other);
	ServConf &operator=(const ServConf &other);

	std::string const&				getServerName(void) const;
	std::string const&				getHost(void) const;
	int								getPort(void) const;
	struct sockaddr_in const&		getSockaddr(void) const;
	std::vector<Location> const&	getLocations(void) const;


private:
	ServConf(void);

	std::string					serverName_;
	std::string					host_;
	int							port_;

	struct sockaddr_in			sockaddr_;

	Location 					defaultLocation_;
	std::vector<Location>		locations_;

};

#endif
