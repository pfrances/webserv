/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:10:23 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 14:43:55 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVCONF_HPP
# define SERVCONF_HPP

# include <string>
# include <map>
# include <vector>
# include <netinet/in.h>
# include <poll.h>
# include "Location.hpp"

class ServConf {

public:
	ServConf(void);
	ServConf(std::string const& confFile);
	~ServConf(void);
	ServConf(ServConf const& other);
	ServConf &operator=(ServConf const& other);

	std::string const&				getServerName(void) const;
	std::string const&				getHost(void) const;
	int								getPort(void) const;
	struct sockaddr_in const&		getSockaddr(void) const;
	std::vector<Location> const&	getLocations(void) const;
	pollfd const&					getPollFd(void) const;
	void							startListen(void) const;

private:
	std::string					serverName_;
	std::string					host_;
	int							port_;

	struct sockaddr_in			sockaddr_;

	Location 					defaultLocation_;
	std::vector<Location>		locations_;
	pollfd						pollfd_;
};

int	stringIpToInt(std::string const& ip);

#endif
