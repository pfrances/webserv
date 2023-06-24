/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/24 14:18:30 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MONITOR_HPP
# define SERVER_MONITOR_HPP

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# include <string>
# include <vector>
# include <poll.h>

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
		void			run(void);
		void			addFd(pollfd const& pollfd);
		void			closePollfd(int fd);

	private:
		std::vector<Server>			serversVec_;
		std::vector<pollfd>			pollfds_;
		void						setServersStartListen(void) const;
};

#endif
