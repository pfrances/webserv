/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/11 23:27:28 by pfrances         ###   ########.fr       */
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

ServerMonitor::ServerMonitor(std::string const& confFileName) :	serversMap_(),
																clientsMap_(),
																responsesMap_(),
																pollfdsVec_() {
	this->addNewPollfd(STDIN_FILENO, POLLIN);
	this->parseConfigFile(confFileName);
}

ServerMonitor::~ServerMonitor(void) {
	std::vector<pollfd>::iterator itPoll = pollfdsVec_.begin();
	std::vector<pollfd>::iterator itePoll = pollfdsVec_.end();
	for (; itPoll != itePoll; itPoll++){
		if (itPoll->fd > 0) {
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

	std::map<int, Response*>::iterator itCgi = cgiResponsesMap_.begin();
	std::map<int, Response*>::iterator iteCgi = cgiResponsesMap_.end();
	for(; itCgi != iteCgi; itCgi++) {
		delete itCgi->second;
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

void	ServerMonitor::parseConfigFile(std::string const& configFileName){
	File				confFile(configFileName);
	std::string const&	conf = confFile.getFileContent();

	std::string::const_iterator	it = conf.begin();
	std::string token = ParseTools::getNextToken(conf, it);
	while (token.empty() == false)
	{
		if (token == "server") {
			std::string serverBlock = ParseTools::extractBlock(conf, it);
			Server *server = new Server(serverBlock);
			int fd = server->getSocketFd();

			this->addNewPollfd(fd, POLLIN);
			this->serversMap_.insert(std::pair<int, Server*>(fd, server));
		}
		else {
			throw ConfigurationException("[Configuration file] Unexpected token: " + token);
		}
		token = ParseTools::getNextToken(conf, it);
	}
	if (this->serversMap_.size() == 0)
		throw ConfigurationException("[Configuration file] No server to configurate");
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
	if (clientfd < 0) {
		return ;
	}

	int enable = 1;
	if (setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		throw IoTroubleException("setsockopt(SO_REUSEADDR) failed");
	}

	this->addNewPollfd(clientfd, POLLIN);
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
		throw ShutdownException("Shutting down...");
	}
}

void	ServerMonitor::handleClientRequest(int fd) {
	std::string msg = this->recvMsg(fd);
	if (msg.empty()) {
		return ;
	}

	std::vector<Request*> resVec = parseMultipleRequest(msg);
	std::vector<Request*>::iterator it = resVec.begin();
	std::vector<Request*>::iterator ite = resVec.end();
	for (; it != ite; it++) {
		Request *req = *it;
		Server *server = this->clientsMap_[fd];
		while (req->isFetched() == false) {
			std::string msg = this->recvMsg(fd);
			req->appendToBody(msg);
		}

		Response *res = server->handleClientRequest(*req);
		if (res) {
			if (res->hasCgiHandler()) {
				int cgiFd = res->getCgiHandler()->getPipeReadFd();
				this->addNewPollfd(cgiFd, POLLIN);
				this->cgiResponsesMap_[cgiFd] = res;
				res->setClientFd(fd);
			} else {
				this->responsesMap_[fd] = res;
				this->addEventToPolfd(fd, POLLOUT);
			}
		}
		delete req;
	}
}

void	ServerMonitor::handleCgiResponse(int fd) {
	Response *res = this->cgiResponsesMap_[fd];

	std::string msg = this->readPipe(fd);

	closeConnection(fd);
	res->killCgiHandler();
	if (msg.empty()) {
		return ;
	}

	res->setRawMessage(msg);
	this->responsesMap_[res->getClientFd()] = res;
	this->addEventToPolfd(res->getClientFd(), POLLOUT);
}

void	ServerMonitor::handleResponseToSend(int fd) {

	Response *res = this->responsesMap_[fd];
	if (res) {
		std::string const& msg = res->getRawMessage();
		this->sendMsg(fd, msg);

		this->RemoveEventToPolfd(fd, POLLOUT);
		delete res;
	}
	this->responsesMap_.erase(fd);
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
			try {
				if (it->revents & (POLLHUP | POLLERR | POLLNVAL)) {
					this->closeConnection(it->fd);
				}
				if (it->revents & POLLIN) {
					if (it->fd == STDIN_FILENO) {
						this->handleUserInput();
					} else if (this->serversMap_.find(it->fd) != this->serversMap_.end()) {
						this->handleNewConnection(it->fd);
					} else if (this->clientsMap_.find(it->fd) != this->clientsMap_.end()) {
						this->handleClientRequest(it->fd);
					} else if (this->cgiResponsesMap_.find(it->fd) != this->cgiResponsesMap_.end()) {
						this->handleCgiResponse(it->fd);
					}
				}
				if (it->revents & POLLOUT) {
					if (this->clientsMap_.find(it->fd) != this->clientsMap_.end()) {
						this->handleResponseToSend(it->fd);
					}
				}
			} catch (IoTroubleException &e) {
				this->closeConnection(it->fd);
			}
		}
	}
}

void	ServerMonitor::closeConnection(int fd) {

	removePollfd(fd);

	if (this->clientsMap_.find(fd) != this->clientsMap_.end()) {
		this->clientsMap_.erase(fd);
	}

	if (this->responsesMap_.find(fd) != this->responsesMap_.end()) {
		delete this->responsesMap_[fd];
		this->responsesMap_.erase(fd);
	}

	if (this->cgiResponsesMap_.find(fd) != this->cgiResponsesMap_.end()) {
		this->cgiResponsesMap_.erase(fd);
	}
}

std::string	ServerMonitor::readPipe(int fd) const {
	char			buff[BUFFER_SIZE + 1];
	std::string		msg;

	while (true) {
		int	bytesRead = read(fd, buff, BUFFER_SIZE);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw IoTroubleException("An issue occured while read from pipe");
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

std::string ServerMonitor::recvMsg(int fd) const {

	char			buff[BUFFER_SIZE + 1];
	std::string		msg;

	while (true) {
		int	bytesRead = recv(fd, buff, BUFFER_SIZE, 0);
		buff[bytesRead] = '\0';
		msg += buff;
		if (bytesRead < 0) {
			throw IoTroubleException("An issue occured while receiving message");
		} else if (bytesRead < BUFFER_SIZE) {
			break ;
		}
	}
	return msg;
}

void	ServerMonitor::sendMsg(int fd, std::string const& msg) const {

	int BytesSent = send(fd, msg.c_str(), msg.length(), 0);
	if (BytesSent < 0)
		throw IoTroubleException("send error");
}

void	ServerMonitor::addNewPollfd(int fd, short events) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd < 0) {
			it->fd = fd;
			it->events = events;
			it->revents = 0;
			return ;
		}
	}
	pollfd pollfd;
	pollfd.fd = fd;
	pollfd.events = events;
	pollfd.revents = 0;
	this->pollfdsVec_.push_back(pollfd);
}

void	ServerMonitor::removePollfd(int fd) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			close(fd);
			it->fd = -1;
		}
	}
}

void	ServerMonitor::addEventToPolfd(int fd, short event) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();

	for (; it != ite; it++) {
		if (it->fd == fd) {
			it->events |= event;
			break;
		}
	}
}

void	ServerMonitor::RemoveEventToPolfd(int fd, short event) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			it->events &= ~event;
			break;
		}
	}
}
