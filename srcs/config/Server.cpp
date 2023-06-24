/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:13:44 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/22 19:20:35 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "Server.hpp"
#include "ParseTools.hpp"
#include "Client.hpp"
#include <stdexcept>
#include <cstdlib>
#include <iostream>

Server::Server(void) {

}

Server::Server(std::string const& serverConf) : serverName_("default"),
												host_("127.0.0.1"),
												port_(8080),
												sockaddr_(),
												pollfd_(),
												defaultLocation_(),
												locations_() {
	this->parseServerConf(serverConf);
	this->sockaddr_.sin_family = AF_INET;
	this->sockaddr_.sin_port = htons(port_);
	this->sockaddr_.sin_addr.s_addr = htonl(stringIpToInt(host_));

	this->pollfd_.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->pollfd_.fd < 0)
		throw std::runtime_error("socket error");
	this->pollfd_.events = POLLIN;
	this->pollfd_.revents = 0;
	if (bind(this->pollfd_.fd, (struct sockaddr *)&sockaddr_, sizeof(sockaddr_)) < 0)
		throw std::runtime_error("bind error");
}

Server::~Server(void) {

}

Server::Server(Server const& other) :	serverName_(other.serverName_),
										host_(other.host_),
										port_(other.port_),
										sockaddr_(other.sockaddr_),
										pollfd_(other.pollfd_),
										defaultLocation_(other.defaultLocation_),
										locations_(other.locations_) {

}

Server &Server::operator=(Server const& other) {
	if (this != &other) {
		this->serverName_ = other.serverName_;
		this->host_ = other.host_;
		this->port_ = other.port_;
		this->sockaddr_ = other.sockaddr_;
		this->defaultLocation_ = other.defaultLocation_;
		this->locations_ = other.locations_;
	}
	return (*this);
}

std::string const& Server::getServerName(void) const {
	return (this->serverName_);
}

std::string const& Server::getHost(void) const {
	return (this->host_);
}

int Server::getPort(void) const {
	return (this->port_);
}

struct sockaddr_in const& Server::getSockaddr(void) const {
	return (this->sockaddr_);
}

std::vector<Location> const& Server::getLocations(void) const {
	return (this->locations_);
}

struct pollfd& Server::getPollFd(void) {
	return (this->pollfd_);
}

std::vector<Client>&	Server::getClients(void) {
	return (this->clients_);
}

Client& Server::getClientAt(int index) {
	return (this->clients_.at(index));
}

int Server::getPollFdFd(void) const {
	return (this->pollfd_.fd);
}

short Server::getPollFdEvents(void) const {
	return (this->pollfd_.events);
}

short Server::getPollFdRevents(void) const {
	return (this->pollfd_.revents);
}

void	Server::setServerName(std::string const& serverName) {
	this->serverName_ = serverName;
}

void	Server::setHost(std::string const& host) {
	this->host_ = host;
}

void	Server::setPort(int port) {
	this->port_ = port;
}

void	Server::setSockaddr(struct sockaddr_in const& sockaddr) {
	this->sockaddr_ = sockaddr;
}

void	Server::setLocations(std::vector<Location> const& locations) {
	this->locations_ = locations;
}

void	Server::setDefaultLocation(Location const& location) {
	this->defaultLocation_ = location;
}

void	Server::addLocation(Location const& location) {
	this->locations_.push_back(location);
}

void	Server::setPollFd(pollfd const& pollfd) {
	this->pollfd_ = pollfd;
}

void	Server::setPollFdEvents(short events) {
	this->pollfd_.events = events;
}

void	Server::setPollFdRevents(short revents) {
	this->pollfd_.revents = revents;
}

void	Server::setPollFdFd(int fd) {
	this->pollfd_.fd = fd;
}

void	Server::addNewClients(ServerMonitor& serverMonitor) {
	if (this->pollfd_.revents & POLLIN) {
		
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		
		pollfd clientFd;
		clientFd.fd = accept(this->pollfd_.fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (clientFd.fd < 0)
			throw std::runtime_error("accept error");
		clientFd.events = POLLIN;
		clientFd.revents = 0;
		
		Client newClient;
		newClient.setPollfd(clientFd);
		this->clients_.push_back(newClient);
		std::cout << __FILE__ << " " << __LINE__ << std::endl;
		serverMonitor.addFd(clientFd);
	}
}

void	Server::clientCommunication(ServerMonitor& serverMonitor) {
	std::vector<Client>::iterator it = this->clients_.begin();
	std::vector<Client>::iterator ite = this->clients_.end();

	for (; it != ite; it++) {
		//it->checkPendingRequests(serverMonitor);
		try {
			it->takeMessagesFromClient(serverMonitor);
			it->sendMessagesToClient();
		} catch (std::exception& e) {
		    serverMonitor.closePollfd(it->getPollfd().fd);
            this->clients_.erase(it);
		}
	}
}

void	Server::startListen(void) const {
	if (listen(this->pollfd_.fd, 10) < 0)
		throw std::runtime_error("listen error");
}

void	Server::parseServerConf(std::string const& serverConf) {
	std::string::const_iterator	it = serverConf.begin();

	std::vector<std::string> tokensVector;
	std::string token = ParseTools::getNextToken(serverConf, it);
	while (token.empty() == false) {
		if (token == "listen") {
			token = ParseTools::getNextToken(serverConf, it);
			this->setPort(std::atoi(token.c_str()));
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: listen: no semicolon.");
			}
		} else if (token == "server_name") {
			token = ParseTools::getNextToken(serverConf, it);
			this->setServerName(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: server_name: no semicolon.");
			}
		} else if (token == "error_page") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(serverConf, it);
			this->defaultLocation_.parseAndAddErrorPages(tokensVector);
		} else if (token == "location") {
			token = ParseTools::getNextToken(serverConf, it);
			std::string locationBlock = ParseTools::extractBlock(serverConf, it);
			this->addLocation(Location(locationBlock, token));
		} else if (token == "client_body_size") {
			token = ParseTools::getNextToken(serverConf, it);
			this->defaultLocation_.setClientMaxBodySize(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: client_body_size: no semicolon.");
			}
		} else if (token == "root") {
			token = ParseTools::getNextToken(serverConf, it);
			this->defaultLocation_.setRoot(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: root: no semicolon.");
			}
		} else {
			throw std::runtime_error("Server::Server: unknown token -->'" + token + "'.");
		}
		token = ParseTools::getNextToken(serverConf, it);
	}

	std::vector<Location>::iterator itLoc = this->locations_.begin();
	std::vector<Location>::iterator iteLoc = this->locations_.end();
	for (; itLoc != iteLoc; itLoc++) {
		itLoc->applyDefaultValues(this->defaultLocation_);
	}
}


int	stringIpToInt(std::string const& ip) {
	std::string::const_iterator it = ip.begin();
	std::string::const_iterator ite = ip.end();
	int result = 0;
	int bit = 0;
	for (; it != ite; it++) {
		if (*it == '.') {
			result = (result << 8) | bit;
			bit = 0;
		} else {
			bit = bit * 10 + (*it - '0');
		}
	}
	result = (result << 8) | bit;
	return result;
}
