/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 17:03:25 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MONITOR_HPP
# define SERVER_MONITOR_HPP

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

# include "ServConf.hpp"
# include <string>
# include <vector>
# include <map>

class Response;

class ServerMonitor
{
	public:
		ServerMonitor(void);
		ServerMonitor(std::string const& confFileName);
		ServerMonitor(ServerMonitor const& other);
		ServerMonitor &operator=(ServerMonitor const& other);
		~ServerMonitor(void);
		void	run(void);

	private:
		// std::vector<ServConf>	servConfs_;
		ServConf					servConfs_;
		std::map<int, Response*>	resMap_;
		std::vector<pollfd>			pollfds_;

		std::string					getMsg(int socketFd);
		void						closeConnection(int socketFd);
};

#endif
