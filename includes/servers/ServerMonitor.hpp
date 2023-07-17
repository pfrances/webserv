/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/17 14:12:20 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MONITOR_HPP
# define SERVER_MONITOR_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# ifndef CLIENT_TIMEOUT
#  define CLIENT_TIMEOUT 30000
# endif

# ifndef CGI_TIMEOUT
#  define CGI_TIMEOUT 1000
# endif

# ifndef POLL_TIMEOUT
#  define POLL_TIMEOUT 100
# endif

# ifndef LISTEN_BACKLOG
#  define LISTEN_BACKLOG 10
# endif

# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <stdexcept>
# include "Timer.hpp"

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
		void					switchLogs(void);

	private:
		std::map<int, Server*>		serversMap_;
		std::map<int, Server*>		clientsMap_;
		std::map<int, CgiHandler*>	cgiHandlersMap_;
		std::map<int, Response*>	responsesMap_;
		std::vector<pollfd>			pollfdsVec_;
		Timer						timer_;
		bool						logsOn_;

		void						parseConfigFile(std::string const& configFileName);

		void						setServersStartListen(void);

		void						handleUserInput(void);
		void						handleNewConnection(int fd);
		void						handleClientRequest(int fd);
		void						handleResponseToSend(int fd);
		void						handleCgiResponse(pollfd const& pollfd);
		void						handleEvents(void);

		std::string					recvMsg(int fd) const;
		std::string					readPipe(int fd) const;
		void						sendBodyToCgi(int fd);
		void						sendMsg(int fd, std::string const& msg) const;

		int							getPollfdsVecIndxFromFd(int fd) const;
		void						addNewPollfd(int fd, short events);
		void						addEventToPollfd(int fd, short event);
		void						RemoveEventToPollfd(int fd, short event);
		void						setEventsToPollfd(int fd, short event);
		void						removePollfd(int fd);

		void						checkTimeOut(void);
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
