/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:20:13 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ConfParser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>

ServerMonitor::ServerMonitor(void) : servConfs_(), pollfds_() {
	pollfds_.push_back(servConfs_.getPollFd());
}

ServerMonitor::ServerMonitor(std::string confFileName) {
	(void)confFileName;
	// servConfs_ = ConfParser::parseMultiServConf(confFileName);
}

ServerMonitor::~ServerMonitor(void) {

}

ServerMonitor::ServerMonitor(const ServerMonitor &other) {
	(void)other;

}

ServerMonitor &ServerMonitor::operator=(const ServerMonitor &other) {
	(void)other;
	if (this != &other) {
	}
	return (*this);
}

void	ServerMonitor::run(void) {
	servConfs_.startListen();
	while (1)
	{
		if (poll(pollfds_.data(), pollfds_.size(), -1) < 0)
			throw std::runtime_error("poll error");
		std::vector<pollfd>::iterator it = pollfds_.begin();
		std::vector<pollfd>::iterator ite = pollfds_.end();
		if (it->revents & POLLIN)
		{
			struct sockaddr_in clientAddr;
			socklen_t clientAddrLen = sizeof(clientAddr);
			pollfd newClient;
			newClient.fd = accept(it->fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
			if (newClient.fd == -1)
				throw std::runtime_error("accept error");
			newClient.events = POLLIN;
			newClient.revents = 0;
			pollfds_.push_back(newClient);
		}
		it++;

		for (; it != ite; it++)
		{
			if (it->revents & POLLIN)
			{
				char buff[1024];
				bzero(buff, 1024);
				if (recv(it->fd, buff, 1024, 0) < 0)
					throw std::runtime_error("recv error");
				if (strlen(buff) == 0) {
					continue;
				}
				std::cout << buff << std::endl;
				Request *req = createRequestFromRequestString(buff);
				if (req == NULL)
					throw std::runtime_error("request error");
				Response res = req->generateResponse(servConfs_);
				this->resMap_.insert(std::pair<int, Response>(it->fd, res));
				// std::string resMsg = res.getRawMessage();
				// std::cout << resMsg << std::endl;
				// if (send(it->fd, resMsg.c_str(), resMsg.length(), 0) < 0)
				// 	throw std::runtime_error("send error");
				it->events |= POLLOUT;
			}
			if (it->revents & POLLOUT) {
				std::string resMsg_ = this->resMap_[it->fd].getRawMessage();
				int BytesSent = send(it->fd, resMsg_.c_str(), resMsg_.length(), 0);
				if (BytesSent < 0) {
					throw std::runtime_error("send error");
				}
				it->events &= ~POLLOUT;
			}
		}
	}
}
/*

poll()



response = request->exec


*/
