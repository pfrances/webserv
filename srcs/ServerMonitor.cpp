/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 17:11:05 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ConfParser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <unistd.h>

ServerMonitor::ServerMonitor(void) : servConfs_(), resMap_(), pollfds_() {
	pollfds_.push_back(servConfs_.getPollFd());
}

ServerMonitor::ServerMonitor(std::string const& confFileName) {
	(void)confFileName;
	// servConfs_ = ConfParser::parseMultiServConf(confFileName);
}

ServerMonitor::~ServerMonitor(void) {

	std::map<int, Response*>::iterator itMap = resMap_.begin();
	std::map<int, Response*>::iterator iteMap = resMap_.end();
	for (; itMap != iteMap; itMap++) {
		delete itMap->second;
	}

	std::vector<pollfd>::iterator itPoll = pollfds_.begin();
	std::vector<pollfd>::iterator itePoll = pollfds_.end();
	for (; itPoll != itePoll; itPoll++){
		close(itPoll->fd);
	}
}

ServerMonitor::ServerMonitor(ServerMonitor const& other) :	servConfs_(other.servConfs_),
															resMap_(other.resMap_),
															pollfds_(other.pollfds_) {

}

ServerMonitor &ServerMonitor::operator=(ServerMonitor const& other) {
	if (this != &other) {
		this->servConfs_ = other.servConfs_;
		this->resMap_ = other.resMap_;
		this->pollfds_ = other.pollfds_;
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
				std::string msg = getMsg(it->fd);
				if (msg.empty())
					continue ;
				Request *req = createRequestFromRequestString(msg);
				if (req == NULL)
					throw std::runtime_error("request error");

				std::cout << "------- request -------" << std::endl;
				std::cout << req->getRawMessage() << std::endl;
				std::cout << "------- end of request -------" << std::endl;

				Response *res = req->execute(servConfs_);
				this->resMap_.insert(std::pair<int, Response*>(it->fd, res));
				it->events |= POLLOUT;
			}

			if (it->revents & POLLOUT) {
				std::string resMsg = this->resMap_[it->fd]->getRawMessage();

				std::cout << "------- response -------" << std::endl;
				std::cout << resMsg << std::endl;
				std::cout << "------- end of response -------" << std::endl;

				int BytesSent = send(it->fd, resMsg.c_str(), resMsg.length(), 0);
				delete this->resMap_[it->fd];
				this->resMap_.erase(it->fd);

				if (BytesSent < 0) {
					throw std::runtime_error("send error");
				}
				it->events &= ~POLLOUT;
			}
		}
	}
}

std::string ServerMonitor::getMsg(int socketFd) {

	char		buff[BUFFER_SIZE + 1];
	std::string	msg;

	while (true) {
		int	bytesRead = recv(socketFd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw std::runtime_error("An issue occured while receiving message");
		} else if (bytesRead == 0) {
			this->closeConnection(socketFd);
			break ;
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

void ServerMonitor::closeConnection(int socketFd) {
	std::vector<pollfd>::iterator itPoll = pollfds_.begin();
	std::vector<pollfd>::iterator itePoll = pollfds_.end();
	for (; itPoll != itePoll; itPoll++) {
		if (itPoll->fd == socketFd) {
			pollfds_.erase(itPoll);
			break ;
		}
	}

	std::map<int, Response*>::iterator itMap = resMap_.begin();
	std::map<int, Response*>::iterator iteMap = resMap_.end();
	for (; itMap != iteMap; itMap++) {
		if (itMap->first == socketFd) {
			delete itMap->second;
			resMap_.erase(itMap);
			break ;
		}
	}
	close(socketFd);
}
