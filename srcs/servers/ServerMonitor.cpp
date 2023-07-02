/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/02 13:03:09 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ParseTools.hpp"
#include "File.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include <iostream>
#include <unistd.h>

ServerMonitor::ServerMonitor(void) {

}

ServerMonitor::ServerMonitor(std::string const& confFileName) :	serversMap_(),
																clientsMap_(),
																responsesMap_(),
																pollfdsVec_() {
	File				confFile(confFileName);
	std::string const&	conf = confFile.getFileContent();

	pollfd pollfd;
	pollfd.fd = STDIN_FILENO;
	pollfd.events = POLLIN;
	pollfd.revents = 0;
	this->pollfdsVec_.push_back(pollfd);

	std::string::const_iterator	it = conf.begin();
	std::string token = ParseTools::getNextToken(conf, it);
	while (token.empty() == false)
	{
		if (token == "server") {
			std::string serverBlock = ParseTools::extractBlock(conf, it);
			Server *server = new Server(serverBlock);

			pollfd.fd = server->getSocketFd();
			pollfd.events = POLLIN;
			pollfd.revents = 0;

			this->pollfdsVec_.push_back(pollfd);
			this->serversMap_.insert(std::pair<int, Server*>(pollfd.fd, server));
		}
		else {
			throw std::runtime_error("ParseTools::parseConfFile: Unexpected token: " + token);
		}
		token = ParseTools::getNextToken(conf, it);
	}
}

ServerMonitor::~ServerMonitor(void) {
	std::vector<pollfd>::iterator itPoll = pollfdsVec_.begin();
	std::vector<pollfd>::iterator itePoll = pollfdsVec_.end();
	for (; itPoll != itePoll; itPoll++){
		if (itPoll->fd != STDIN_FILENO) {
			close(itPoll->fd);
		}
	}

	std::map<int, Server*>::iterator itServ = serversMap_.begin();
	std::map<int, Server*>::iterator iteServ = serversMap_.end();
	for (; itServ != iteServ; itServ++) {
		delete itServ->second;
	}

	std::map<int, Response*>::iterator itRes = responsesMap_.begin();
	std::map<int, Response*>::iterator iteRes = responsesMap_.end();
	for(; itRes != iteRes; itRes++) {
		delete itRes->second;
	}
}

ServerMonitor::ServerMonitor(ServerMonitor const& other) :	serversMap_(other.serversMap_),
															clientsMap_(other.clientsMap_),
															responsesMap_(other.responsesMap_),
															pollfdsVec_(other.pollfdsVec_) {

}

ServerMonitor &ServerMonitor::operator=(ServerMonitor const& other) {
	if (this != &other) {
		this->serversMap_ = other.serversMap_;
		this->clientsMap_ = other.clientsMap_;
		this->responsesMap_ = other.responsesMap_;
		this->pollfdsVec_ = other.pollfdsVec_;
	}
	return (*this);
}

void	ServerMonitor::setServersStartListen(void) const {
	std::map<int, Server*>::const_iterator it = serversMap_.begin();
	std::map<int, Server*>::const_iterator ite = serversMap_.end();
	for (; it != ite; it++) {
		Server *server = it->second;
		server->startListen();
	}
}

void	ServerMonitor::handleNewConnection(int fd) {
	int clientfd = this->serversMap_[fd]->acceptNewClient();
	if (fd < 0) {
		throw std::runtime_error("accept error");
	}

	int enable = 1;
	if (setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
	}

	this->pollfdsVec_.push_back(pollfd());
	this->pollfdsVec_.back().fd = clientfd;
	this->pollfdsVec_.back().events = POLLIN;
	this->pollfdsVec_.back().revents = 0;
	this->clientsMap_.insert(std::pair<int, Server*>(clientfd, this->serversMap_[fd]));
}

int	ServerMonitor::getPollfdsVecIndxFromFd(int fd) const {
	std::vector<pollfd>::const_iterator it = pollfdsVec_.begin();
	std::vector<pollfd>::const_iterator ite = pollfdsVec_.end();
	for (int i = 0; it != ite; it++, i++) {
		if (it->fd == fd) {
			return (i);
		}
	}
	return (-1);
}

void	ServerMonitor::handleUserInput(void) {
	std::string msg;
	std::getline(std::cin, msg);
	if (msg == "exit") {
		throw std::runtime_error("Shutting down...");
	}
}

void	ServerMonitor::handleClientRequest(pollfd& pollfd) {
	int	fd = pollfd.fd;
	try {
		std::string msg = this->recvMsg(fd);
		if (msg.empty()) {
			return ;
		}
		Request req(msg, fd);
		Server *server = this->clientsMap_[fd];
		Response *res = server->handleClientRequest(req);
		if (res) {
			this->responsesMap_[fd] = res;
			pollfd.events |= POLLOUT;
		}
	} catch (std::exception& e) {
		this->pollfdsVec_.erase(this->pollfdsVec_.begin() + this->getPollfdsVecIndxFromFd(fd));
		this->clientsMap_.erase(fd);
		close(fd);
	}
}

void	ServerMonitor::handleResponseToSend(pollfd& pollfd) {

	int	fd = pollfd.fd;
	Response *res = this->responsesMap_[fd];
	if (res) {
		std::string const& msg = res->getRawMessage();
		this->sendMsg(fd, msg);

		this->responsesMap_.erase(fd);
		pollfd.events &= ~POLLOUT;
		delete res;
	}
}

void	ServerMonitor::run(void) {
	this->setServersStartListen();
	while (1)
	{
		if (poll(pollfdsVec_.data(), pollfdsVec_.size(), -1) < 0) {
			throw std::runtime_error("poll error");
		}

		std::vector<pollfd>::iterator it = pollfdsVec_.begin();
		std::vector<pollfd>::iterator ite = pollfdsVec_.end();
		for (; it != ite; it++) {
			if (it->revents & POLLIN) {
				if (it->fd == STDIN_FILENO) {
					this->handleUserInput();
				} else if (this->serversMap_.find(it->fd) != this->serversMap_.end()) {
					this->handleNewConnection(it->fd);
				} else if (this->clientsMap_.find(it->fd) != this->clientsMap_.end()) {
					this->handleClientRequest(*it);
				}
			}
			if (it->revents & POLLOUT) {
				if (this->clientsMap_.find(it->fd) != this->clientsMap_.end()) {
					this->handleResponseToSend(*it);
				}
			}
		}
	}
}

void	ServerMonitor::removePollfd(int fd) {
	std::vector<pollfd>::iterator it = pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			pollfdsVec_.erase(it);
			close(fd);
			break ;
		}
	}
}

std::string ServerMonitor::recvMsg(int fd) const {

	char			buff[BUFFER_SIZE + 1];
	std::string		msg;

	while (true) {
		int	bytesRead = recv(fd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw std::runtime_error("An issue occured while receiving message");
		} else if (bytesRead == 0) {
			throw std::runtime_error("End of communication");
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

void	ServerMonitor::sendMsg(int fd, std::string const& msg) const {

	int BytesSent = send(fd, msg.c_str(), msg.length(), 0);
	if (BytesSent < 0)
		throw std::runtime_error("send error");
}
