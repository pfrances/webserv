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
# include <netinet/in.h>
# include "Location.hpp"

class Request;
class Response;
class File;

class Server {
	public:
		Server(std::string const& serverBlock);
		~Server(void);
		Server(Server const& other);
		Server &operator=(Server const& other);

		std::string const&		getServerName(void) const;
		std::string const&		getHost(void) const;
		short					getPort(void) const;
		int						getSocketFd(void) const;

		void					startListen(void) const;

		int						acceptNewClient(void);
		Response*				handleClientRequest(Request const& req) const;

		bool					isCgiRequest(Request const& req) const;

	private:
		Server(void);
		void					parseServerConf(std::string const& serverConf);
		void					setPort(std::string const& portStr);
		void					addLocation(Location* location);

		Location*				getCorrespondingLocation(std::string const& uri) const;

		Response*				handleError(int statusCode, Location *location) const;
		std::string				setFileListHtmlToReqBody(std::map<std::string, std::string> const& filesList,
														Request const& req) const;
		Response*				handleIndexing(File const& file, Request const& req, Location *location) const;
		Response*				handleGetRequest(Request const& req) const;
		Response*				handlePostRequest(Request const& req) const;
		Response*				handleDeleteRequest(Request const& req) const;
		Response*				handleUnknownRequest(Request const& req) const;

		std::string				serverName_;
		std::string				host_;
		int						port_;

		sockaddr_in				sockaddr_;
		int						socketFd_;

		std::map<std::string,
			Location*>			locationsMap_;
};

int	stringIpToInt(std::string const& ip);

#endif
