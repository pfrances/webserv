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

#include "Server.hpp"
#include "ParseTools.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "File.hpp"
#include <stdexcept>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <iostream>

Server::Server(std::string const& serverConf) :	serverName_("default"),
												host_("127.0.0.1"),
												port_(8080),
												sockaddr_(),
												socketFd_(-1),
												locationsMap_() {
	this->parseServerConf(serverConf);
	this->sockaddr_.sin_family = AF_INET;
	this->sockaddr_.sin_port = htons(port_);
	this->sockaddr_.sin_addr.s_addr = htonl(stringIpToInt(host_));

	this->socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socketFd_ < 0)
		throw std::runtime_error("socket error");

	if (bind(this->socketFd_, (struct sockaddr *)&sockaddr_, sizeof(sockaddr_)) < 0)
		throw std::runtime_error("bind error");
}

Server::~Server(void) {
	std::map<std::string, Location*>::iterator it = this->locationsMap_.begin();
	std::map<std::string, Location*>::iterator ite = this->locationsMap_.end();
	for (; it != ite; it++) {
		delete it->second;
	}
}

Server::Server(Server const& other) :	serverName_(other.serverName_),
										host_(other.host_),
										port_(other.port_),
										sockaddr_(other.sockaddr_),
										socketFd_(other.socketFd_),
										locationsMap_(other.locationsMap_) {

}

Server &Server::operator=(Server const& other) {
	if (this != &other) {
		this->serverName_ = other.serverName_;
		this->host_ = other.host_;
		this->port_ = other.port_;
		this->sockaddr_ = other.sockaddr_;
		this->locationsMap_ = other.locationsMap_;
	}
	return (*this);
}

std::string const& Server::getServerName(void) const {
	return (this->serverName_);
}

std::string const& Server::getHost(void) const {
	return (this->host_);
}

short Server::getPort(void) const {
	return (this->port_);
}

void Server::setPort(std::string const& portStr) {
	int port = std::atoi(portStr.c_str());
	if (port < 0 || port > std::numeric_limits<unsigned short>::max()) {
		throw std::runtime_error("Server::setPort: invalid port");
	}
	this->port_ = static_cast<short>(port);
}

int	Server::getSocketFd(void) const {
	return (this->socketFd_);
}

void	Server::addLocation(Location* location) {
	this->locationsMap_.insert(std::pair<std::string, Location*>(location->getPath(), location));
}

int	Server::acceptNewClient(void) {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	return accept(this->socketFd_, (struct sockaddr *)&clientAddr, &clientAddrLen);
}

void	Server::startListen(void) const {
	if (listen(this->socketFd_, 10) < 0) {
		throw std::runtime_error("listen error");
	}
}

void	Server::parseServerConf(std::string const& serverConf) {
	std::string::const_iterator	it = serverConf.begin();

	Location defaultLocation;
	std::vector<std::string> tokensVec;
	std::string token = ParseTools::getNextToken(serverConf, it);
	while (token.empty() == false) {
		if (token == "listen") {
			token = ParseTools::getNextToken(serverConf, it);
			this->setPort(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: listen: no semicolon.");
			}
		} else if (token == "server_name") {
			token = ParseTools::getNextToken(serverConf, it);
			this->serverName_ = token;
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: server_name: no semicolon.");
			}
		} else if (token == "error_page") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(serverConf, it);
			defaultLocation.parseAndAddErrorPages(tokensVec);
		} else if (token == "location") {
			token = ParseTools::getNextToken(serverConf, it);
			std::string locationBlock = ParseTools::extractBlock(serverConf, it);
			this->addLocation(new Location(locationBlock, token));
		} else if (token == "client_body_size") {
			token = ParseTools::getNextToken(serverConf, it);
			defaultLocation.setClientMaxBodySize(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: client_body_size: no semicolon.");
			}
		} else if (token == "root") {
			token = ParseTools::getNextToken(serverConf, it);
			defaultLocation.setRoot(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw std::runtime_error("Server::Server: root: no semicolon.");
			}
		} else {
			throw std::runtime_error("Server::Server: unknown token -->'" + token + "'.");
		}
		token = ParseTools::getNextToken(serverConf, it);
	}

	std::map<std::string, Location*>::iterator itLoc = this->locationsMap_.begin();
	std::map<std::string, Location*>::iterator iteLoc = this->locationsMap_.end();
	for (; itLoc != iteLoc; itLoc++) {
		Location* location = itLoc->second;
		location->applyDefaultValues(defaultLocation);
	}
}

Response*	Server::handleGetRequest(Request const& req) const {

	Location *location = getCorrespondingLocation(req.getUri());
	if (location == NULL) {
		throw std::runtime_error("Server::handleGetRequest: no location found");
	}

	std::string const& path = location->getRoot() + req.getUri();
	std::cout << "path = " << path << std::endl;
	File file(path);
	std::string const& fileContent = file.getFileContent();
	std::ostringstream oss;
	oss << fileContent.length();
	std::cout << "fileContent = " << fileContent << std::endl;
	std::cout << "fileContent.length() = " << fileContent.length() << std::endl;
	std::cout << req.getRawMessage() << std::endl;
	Response *res = new Response();
	res->setStartLine("HTTP/1.1 200 OK");
	if (file.getExtension() == "html") {
		res->setSingleHeader("Content-Type", "text/html");
	} else if (file.getExtension() == "jpg") {
		res->setSingleHeader("Content-Type", "image/jpeg");
	}
	res->setSingleHeader("Content-Length", oss.str());
	res->setBody(fileContent);
	return res;
}

Response*	Server::handlePostRequest(Request const& req) const {
	(void)req;
	return NULL;
}

Response*	Server::handleDeleteRequest(Request const& req) const {
	(void)req;
	return NULL;
}

Response*	Server::handleUnknownRequest(Request const& req) const {
	(void)req;
	return NULL;
}

Response*	Server::handleClientRequest(Request const& req) const {
	std::string const& method = req.getMethod();
	if (method == "GET") {
		return this->handleGetRequest(req);
	} else if (method == "POST") {
		return this->handlePostRequest(req);
	} else if (method == "DELETE") {
		return this->handleDeleteRequest(req);
	}
	return this->handleUnknownRequest(req);
}

Location*	Server::getCorrespondingLocation(std::string const& uri) const {

	std::string tmp = uri;

	while (tmp.size() > 0)
	{
		std::map<std::string, Location*>::const_iterator it = this->locationsMap_.find(tmp);
		if (it != this->locationsMap_.end()) {
			return it->second;
		}
		size_t pos = tmp.find_last_of('/');
		if (pos == std::string::npos) {
			break;
		}
		tmp = tmp.substr(0, pos);
	}
	if (this->locationsMap_.find("/") != this->locationsMap_.end()) {
		return this->locationsMap_.find("/")->second;
	}
	return NULL;
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
