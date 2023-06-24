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
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <unistd.h>

ServerMonitor::ServerMonitor(void) {

}

ServerMonitor::ServerMonitor(std::string const& confFileName) {
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
			std::cout << server.getPollFd().fd << std::endl;
			Server *tmp = &this->serversVec_.at(0);
			std::cout << tmp << " " << &server << std::endl;
			std::cout << tmp->getPollFdFd() << std::endl;
			std::cout << tmp->getPort() << std::endl;
			std::cout << tmp->getHost() << std::endl;
			std::cout << tmp->getServerName() << std::endl;
		}
		else {
			throw std::runtime_error("ParseTools::parseConfFile: Unexpected token: " + token);
		}
		token = ParseTools::getNextToken(conf, it);
	}
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

ServerMonitor::ServerMonitor(ServerMonitor const& other) :	serversVec_(other.serversVec_),
															resMap_(other.resMap_),
															pollfds_(other.pollfds_) {

}

ServerMonitor &ServerMonitor::operator=(ServerMonitor const& other) {
	if (this != &other) {
		this->serversVec_ = other.serversVec_;
		this->resMap_ = other.resMap_;
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
	std::cout << "ServerMonitor::run: start" << std::endl;
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

				Response *res = req->execute(serversVec_.at(0));
				this->resMap_.insert(std::pair<int, Response*>(it->fd, res));
				it->events |= POLLOUT;
			}

			if (it->revents & POLLOUT) {
				std::string resMsg = this->resMap_[it->fd]->getRawMessage();

				std::cout << "------- response -------" << std::endl;
				std::cout << "fd: " << it->fd << "-->" << resMsg << std::endl;
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

// void	ServerMonitor::run(void) {
// 	servConfs_.startListen();
// 	while (1)
// 	{
// 		if (poll(pollfds_.data(), pollfds_.size(), -1) < 0)
// 			throw std::runtime_error("poll error");


// 		std::vector<Server>::iterator it = this->serversVec_.begin();
// 		std::vector<Server>::iterator ite = this->serversVec_.end();
// 		while (it != ite)
// 		{
// 			if (it->getPollFd().revents & POLLIN)
// 			{
// 				struct sockaddr_in clientAddr;
// 				socklen_t clientAddrLen = sizeof(clientAddr);
// 				pollfd newClient;
// 				newClient.fd = accept(it->fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
// 				if (newClient.fd == -1)
// 					throw std::runtime_error("accept error");
// 				newClient.events = POLLIN;
// 				newClient.revents = 0;
// 				pollfds_.push_back(newClient);

// 			}
// 			std::vector<Client>::iterator itClient = it->getClients().begin();
// 			std::vector<Client>::iterator iteClient = it->getClients().begin();
// 			while(itClient != iteClient) {
// 				if (Client REsquest->event) {

// 				}

// 				if (it->revents & POLLIN)
// 				{
// 					std::string msg = getMsg(it->fd);
// 					if (msg.empty())
// 						continue ;
// 					Request *req = createRequestFromRequestString(msg);
// 					if (req == NULL)
// 						throw std::runtime_error("request error");

// 					std::cout << "------- request -------" << std::endl;
// 					std::cout << req->getRawMessage() << std::endl;
// 					std::cout << "------- end of request -------" << std::endl;

// 					Response *res = req->execute(servConfs_);
// 					this->resMap_.insert(std::pair<int, Response*>(it->fd, res));
// 					it->events |= POLLOUT;
// 				}

// 				if (it->revents & POLLOUT) {
// 					std::string resMsg = this->resMap_[it->fd]->getRawMessage();

// 					std::cout << "------- response -------" << std::endl;
// 					std::cout << "fd: " << it->fd << "-->" << resMsg << std::endl;
// 					std::cout << "------- end of response -------" << std::endl;

// 					int BytesSent = send(it->fd, resMsg.c_str(), resMsg.length(), 0);
// 					delete this->resMap_[it->fd];
// 					this->resMap_.erase(it->fd);

// 					if (BytesSent < 0) {
// 						throw std::runtime_error("send error");
// 					}
// 					it->events &= ~POLLOUT;
// 				}

// 				itClient++;
// 			}
// 			it++;
// 		}
// 		for (; it != ite; it++)
// 		{
// 		}
// 	}
// }

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

/*
ServMonitor:
vector<servConf>
vector<pollfd>

Client:
vector<request*>
vector<response*>
pollfd

ServerConf
vector<Client>
pollfd

Request:
vector<pollfd> / pollfd -->fileToRead

Response:
///

while() {
	for (vector->serv)
	servLoop {
		pollf check --> accept
		for (vector Client)
		ClientLoop {
			pollfd --> getmsg -- > create request　+ add poll out

			Request {
				if fd ready
				--> newResponse = process() ==> get response
				delete request + ClientのVectorから外す
				if needed, remove fileToReadFd from ServerMonitor Vector<pollfd>
			}

			Response {
				if fd ready
				--> send　
				delete　+ ClientのVectorから外す
			}
		}
	}

	--> ServerMonitor vector<pollfd>の更新 {
		Server {
			Client {
				Request {

				}
				Response {

				}
			}
		}
	}

	Req* = createReq()

	try {
		response res = req->exect(); --> POST / DELETE

		client.addREsponse(res);
		req->getpendingFds -- > remove from ServMonitor.pollfds_
		delete req;
	} catch ( execptNOTREADY &e) {
		pollfds_.pushback(e.getfd());
	} catch ( FileDoesntExist )

	if fd.revents & POLLIN {
		return createRes();
	} else {
		throw (fd);
	}
}

Servmonitor -- > Client --> Request -- > pendingsfd

*/
