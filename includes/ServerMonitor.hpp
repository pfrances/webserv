/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:00:29 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:20:10 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MONITOR_HPP
# define SERVER_MONITOR_HPP

# include <string>
# include <vector>
# include <map>
# include "ServConf.hpp"
class Response;

class ServerMonitor
{
	public:
		ServerMonitor(void);
		ServerMonitor(std::string confFileName);
		ServerMonitor(const ServerMonitor &other);
		ServerMonitor &operator=(const ServerMonitor &other);
		~ServerMonitor(void);
		void	run(void);

	private:
		// std::vector<ServConf>	servConfs_;
		std::map<int, Response>	resMap_;
		std::vector<pollfd>		pollfds_;
		ServConf	servConfs_;
};

#endif
