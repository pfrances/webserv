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

class Location;
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
		std::map<std::string,
			Location*> const&	getLocationsMap(void) const;
		size_t					getClientLastRequestTime(int clientFd) const;

		void					startListen(void);

		int						acceptNewClient(void);
		Response*				handleClientRequest(Request const& req);

		void					prepareSocket(void);
		void					addSubServer(Server *subServ);
		Response*				handleError(int statusCode) const;
		void					setClientLastRequestTime(int clientFd, size_t time);
		void					removeClient(int clientFd);


	private:
		Server(void);
		void					parseServerConf(std::string const& serverConf);
		void					setPort(std::string const& portStr);
		void					addLocation(Location* location);

		Location*				findLocation(std::map<std::string, Location*> const& locMap,
												std::string const& uri);
		Location*				getCorrespondingLocation(Request const& req);

		std::string				setFileListHtmlToReqBody(std::map<std::string, std::string> const& filesList,
														Request const& req) const;
		Response*				handleIndexing(File const& file, Request const& req, Location *location) const;
		Response*				handleGetRequest(Request const& req, Location *location) const;
		Response*				handlePostRequest(Request const& req, Location *location) const;
		Response*				handleLogPostRequest(Request const& req, Location *location) const;
		Response*				handleDeleteRequest(Request const& req, Location *location) const;

		Response*				handleCgiRequest(Request const& req, Location *location) const;

		Response*				handleRedirection(Request const& req, Location *location) const;
		Response*				handleError(int statusCode, Location *location) const;

		std::string				serverName_;
		std::string				host_;
		int						port_;

		sockaddr_in				sockaddr_;
		int						socketFd_;

		std::map<std::string,
			Location*>			locationsMap_;
		std::map<std::string,
					Server*>	subServMap_;
		std::map<int, size_t>	clientLastRequestTimeMap_;
};

int	stringIpToInt(std::string const& ip);

#endif
