/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/24 14:51:27 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ParseTools.hpp"
#include "File.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <iostream>
#include <unistd.h>

ServerMonitor::ServerMonitor(void) {

}

ServerMonitor::ServerMonitor(std::string const& confFileName) :	serversVec_(),
																pollfds_() {
	File				confFile(confFileName);
	std::string const&	conf = confFile.getFileContent();

	std::string::const_iterator	it = conf.begin();

	std::string token = ParseTools::getNextToken(conf, it);
	while (token.empty() == false)
	{
		if (token == "server") {
			std::string serverBlock = ParseTools::extractBlock(conf, it);
			Server server(serverBlock);
			this->serversVec_.push_back(server);
			this->pollfds_.push_back(server.getPollFd());
		}
		else {
			throw std::runtime_error("ParseTools::parseConfFile: Unexpected token: " + token);
		}
		token = ParseTools::getNextToken(conf, it);
	}
}

ServerMonitor::~ServerMonitor(void) {
	std::vector<pollfd>::iterator it = pollfds_.begin();
	std::vector<pollfd>::iterator ite = pollfds_.end();
	for (; it != ite; it++){
		close(it->fd);
	}
}

ServerMonitor::ServerMonitor(ServerMonitor const& other) :	serversVec_(other.serversVec_),
															pollfds_(other.pollfds_) {

}

ServerMonitor &ServerMonitor::operator=(ServerMonitor const& other) {
	if (this != &other) {
		this->serversVec_ = other.serversVec_;
		this->pollfds_ = other.pollfds_;
	}
	return (*this);
}

void	ServerMonitor::setServersStartListen(void) const {
	std::vector<Server>::const_iterator it = serversVec_.begin();
	std::vector<Server>::const_iterator ite = serversVec_.end();
	for (; it != ite; it++) {
		it->startListen();
	}
}

void	ServerMonitor::run(void) {
	this->setServersStartListen();
	while (1)
	{
		if (poll(pollfds_.data(), pollfds_.size(), -1) < 0)
			throw std::runtime_error("poll error");
		std::vector<Server>::iterator it = serversVec_.begin();
		std::vector<Server>::iterator ite = serversVec_.end();
		for (; it != ite; it++)
		{
			std::cout << this->pollfds_.at(0).revents << std::endl;
			std::cout << it->getPollFd().fd << std::endl;
			std::cout << it->getPollFd().events << std::endl;
			std::cout << it->getPollFd().revents << std::endl;
			return ;
			it->addNewClients(*this);
			it->clientCommunication(*this);
		}
	}
}

void ServerMonitor::closePollfd(int fd) {
	std::vector<pollfd>::iterator it = pollfds_.begin();
	std::vector<pollfd>::iterator ite = pollfds_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			pollfds_.erase(it);
			break ;
		}
	}
	close(fd);
}

void ServerMonitor::addFd(pollfd const& fd) {
	this->pollfds_.push_back(fd);
}
