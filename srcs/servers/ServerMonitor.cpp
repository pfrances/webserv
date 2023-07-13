/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 12:51:14 by pfrances         ###   ########.fr       */
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

	std::map<int, CgiHandler*>::iterator itCgi = cgiHandlersMap_.begin();
	std::map<int, CgiHandler*>::iterator iteCgi = cgiHandlersMap_.end();
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
			std::map<int, Server*>::iterator it = this->serversMap_.begin();
			std::map<int, Server*>::iterator ite = this->serversMap_.end();
			for (; it != ite; it++) {
				if (it->second->getPort() == server->getPort()) {
					it->second->addSubServer(server);
					break;
				}
			}
			if (it == ite) {
				server->prepareSocket();
				this->serversMap_[server->getSocketFd()] = server;
			}
		}
		else {
			throw ConfigurationException("[Configuration file] Unexpected token: " + token);
		}
		token = ParseTools::getNextToken(conf, it);
	}
	if (this->serversMap_.size() == 0)
		throw ConfigurationException("[Configuration file] No server to configurate");
}

void	ServerMonitor::setServersStartListen(void) {
	std::map<int, Server*>::const_iterator it = serversMap_.begin();
	std::map<int, Server*>::const_iterator ite = serversMap_.end();
	for (; it != ite; it++) {
		Server *server = it->second;
		int fd = server->getSocketFd();

		this->addNewPollfd(fd, POLLIN);
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
				CgiHandler *cgi = res->getCgiHandler();
				int cgiFd = cgi->getPipeReadFd();
				this->addNewPollfd(cgiFd, POLLIN);
				this->cgiHandlersMap_[cgiFd] = cgi;
				cgi->setClientFd(fd);
				delete res;
			} else {
				this->responsesMap_[fd] = res;
				this->addEventToPolfd(fd, POLLOUT);
			}
		}
		delete req;
	}
}

void	ServerMonitor::handleCgiResponse(int fd) {

	std::string msg = this->readPipe(fd);

	CgiHandler *cgi = this->cgiHandlersMap_[fd];
	int clientFd = cgi->getClientFd();
	closeConnection(fd);

	if (msg.empty()) {
		return ;
	}

	Response *res = new Response(msg);
	this->responsesMap_[clientFd] = res;
	this->addEventToPolfd(clientFd, POLLOUT);
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

void ServerMonitor::run() {
	this->setServersStartListen();
	while (1) {
		if (poll(pollfdsVec_.data(), pollfdsVec_.size(), -1) < 0) {
			throw std::runtime_error("poll error");
		}

		for (size_t i = 0; i < pollfdsVec_.size(); i++) {
			try {
				if (pollfdsVec_[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
					this->closeConnection(pollfdsVec_[i].fd);
				}
				if (pollfdsVec_[i].revents & POLLIN) {
					if (pollfdsVec_[i].fd == STDIN_FILENO) {
						this->handleUserInput();
					} else if (this->serversMap_.find(pollfdsVec_[i].fd) != this->serversMap_.end()) {
						this->handleNewConnection(pollfdsVec_[i].fd);
					} else if (this->clientsMap_.find(pollfdsVec_[i].fd) != this->clientsMap_.end()) {
						this->handleClientRequest(pollfdsVec_[i].fd);
					} else if (this->cgiHandlersMap_.find(pollfdsVec_[i].fd) != this->cgiHandlersMap_.end()) {
						this->handleCgiResponse(pollfdsVec_[i].fd);
					}
				}
				if (pollfdsVec_[i].revents & POLLOUT) {
					if (this->clientsMap_.find(pollfdsVec_[i].fd) != this->clientsMap_.end()) {
						this->handleResponseToSend(pollfdsVec_[i].fd);
					}
				}
			} catch (IoTroubleException &e) {
				this->closeConnection(pollfdsVec_[i].fd);
			}
		}
	}
}

void	ServerMonitor::closeConnection(int fd) {

	removePollfd(fd);

	if (this->clientsMap_.find(fd) != this->clientsMap_.end()) {
		this->clientsMap_.erase(fd);
	}

	if (this->cgiHandlersMap_.find(fd) != this->cgiHandlersMap_.end()) {
		delete this->cgiHandlersMap_[fd];
		this->cgiHandlersMap_.erase(fd);
	}

	if (this->responsesMap_.find(fd) != this->responsesMap_.end()) {
		delete this->responsesMap_[fd];
		this->responsesMap_.erase(fd);
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
	this->pollfdsVec_.push_back(pollfd());
	this->pollfdsVec_.back().fd = fd;
	this->pollfdsVec_.back().events = events;
	this->pollfdsVec_.back().revents = 0;
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
