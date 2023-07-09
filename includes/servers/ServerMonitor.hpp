/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/09 14:40:06 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MONITOR_HPP
# define SERVER_MONITOR_HPP

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <stdexcept>


class CgiHandler;
class Response;
class Server;

class ServerMonitor
{
	public:
		ServerMonitor(std::string const& confFileName);
		ServerMonitor(ServerMonitor const& other);
		ServerMonitor &operator=(ServerMonitor const& other);
		~ServerMonitor(void);

		void					run(void);


		void					closeConnection(int fd);

	private:
		std::map<int, Server*>		serversMap_;
		std::map<int, Server*>		clientsMap_;
		std::map<int, CgiHandler*>	cgiHandlersMap_;
		std::map<int, Response*>	responsesMap_;
		std::vector<pollfd>			pollfdsVec_;

		void						parseConfigFile(std::string const& configFileName);

		void						setServersStartListen(void) const;

		void						handleUserInput(void);
		void						handleNewConnection(int fd);
		void						handleClientRequest(int fd);
		void						handleResponseToSend(int fd);
		void						handleCgiResponse(int fd);

		std::string					recvMsg(int fd) const;
		void						sendMsg(int fd, std::string const& msg) const;

		int							getPollfdsVecIndxFromFd(int fd) const;
		void						addNewPollfd(int fd, short events);
		void						addEventToPolfd(int fd, short event);
		void						RemoveEventToPolfd(int fd, short event);
};

class IoTroubleException : public std::runtime_error {
	public:
		IoTroubleException(std::string const& msg) : std::runtime_error(msg) {}
};

class ShutdownException : public std::runtime_error {
	public:
		ShutdownException(std::string const& msg) : std::runtime_error(msg) {}
};

#endif
