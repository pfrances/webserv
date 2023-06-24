/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                       :+:      :+:    :+:   */
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

class Server {

public:
	Server(void);
	Server(std::string const& serverBlock);
	~Server(void);
	Server(Server const& other);
	Server &operator=(Server const& other);

	std::string const&				getServerName(void) const;
	std::string const&				getHost(void) const;
	int								getPort(void) const;
	struct sockaddr_in const&		getSockaddr(void) const;
	std::vector<Location> const&	getLocations(void) const;
	struct pollfd const&			getPollFd(void) const;
	int								getPollFdFd(void) const;
	short							getPollFdEvents(void) const;
	short							getPollFdRevents(void) const;
	void							startListen(void) const;

	void							setServerName(std::string const& serverName);
	void							setHost(std::string const& host);
	void							setPort(int port);
	void							setSockaddr(struct sockaddr_in const& sockaddr);

	void							setPollFd(pollfd const& pollfd);
	void							setPollFdFd(int fd);
	void							setPollFdEvents(short events);
	void							setPollFdRevents(short revents);


	void							setDefaultLocation(Location const& location);
	void							setLocations(std::vector<Location> const& locations);
	void							addLocation(Location const& location);

private:
	void						parseServerConf(std::string const& serverConf);
	std::string					serverName_;
	std::string					host_;
	int							port_;

	struct sockaddr_in			sockaddr_;
	struct pollfd				pollfd_;

	Location					defaultLocation_;
	std::vector<Location>		locations_;
};

int	stringIpToInt(std::string const& ip);

#endif
