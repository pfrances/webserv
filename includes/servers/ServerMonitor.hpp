/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/01 19:24:15 by pfrances         ###   ########.fr       */
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

class CgiHandler;
class Response;
class Server;

class ServerMonitor
{
	public:
		ServerMonitor(void);
		ServerMonitor(std::string const& confFileName);
		ServerMonitor(ServerMonitor const& other);
		ServerMonitor &operator=(ServerMonitor const& other);
		~ServerMonitor(void);

		void					run(void);


		void					removePollfd(int fd);

	private:
		std::map<int, Server*>		serversMap_;
		std::map<int, Server*>		clientsMap_;
		std::map<int, CgiHandler*>	cgiHandlersMap_;
		std::map<int, Response*>	responsesMap_;
		std::vector<pollfd>			pollfdsVec_;

		void						setServersStartListen(void) const;

		void						handleUserInput(void);
		void						handleNewConnection(int fd);
		void						handleClientRequest(pollfd& pollfd);
		void						handleResponseToSend(pollfd& pollfd);
		void						handleCgiResponse(pollfd& pollfd);

		std::string					recvMsg(int fd) const;
		void						sendMsg(int fd, std::string const& msg) const;

		int							getPollfdsVecIndxFromFd(int fd) const;
};

#endif
