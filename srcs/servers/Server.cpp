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

	int enable = 1;
	if (setsockopt(this->socketFd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
	}

	if (bind(this->socketFd_, (struct sockaddr *)&sockaddr_, sizeof(sockaddr_)) < 0) {
		throw std::runtime_error("bind error");
	}
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
	int port = ParseTools::stringToInt(portStr);
	if (port < 0 || port > std::numeric_limits<unsigned short>::max()) {
		throw ConfigurationException("Server::setPort: invalid port");
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
		throw std::runtime_error(this->getHost() + ":" + ParseTools::intToString(this->getPort()) + " listen error");
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
				throw ConfigurationException("[Configuration Server] listen: no semicolon.");
			}
		} else if (token == "server_name") {
			token = ParseTools::getNextToken(serverConf, it);
			this->serverName_ = token;
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw ConfigurationException("[Configuration Server] server_name: no semicolon.");
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
				throw ConfigurationException("[Configuration Server] client_body_size: no semicolon.");
			}
		} else if (token == "index") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(serverConf, it);
			defaultLocation.setIndex(tokensVec);
		} else if (token == "root") {
			token = ParseTools::getNextToken(serverConf, it);
			defaultLocation.setRoot(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw ConfigurationException("[Configuration Server] root: no semicolon.");
			}
		} else if (token == "autoindex") {
				token = ParseTools::getNextToken(serverConf, it);
			if (token == "on") {
				defaultLocation.setAutoIndex(true);
			} else if (token == "off") {
				defaultLocation.setAutoIndex(false);
			} else {
				throw ConfigurationException("[Configuration Server] autoindex: invalid argument.");
			}
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw ConfigurationException("[Configuration Server] autoindex: no semicolon.");
			}
		} else {
			throw ConfigurationException("[Configuration Server] unknown token " + token);
		}
		token = ParseTools::getNextToken(serverConf, it);
	}

	std::map<std::string, Location*>::iterator itLoc = this->locationsMap_.begin();
	std::map<std::string, Location*>::iterator iteLoc = this->locationsMap_.end();
	if (itLoc == iteLoc) {
		this->addLocation(new Location(defaultLocation));
	} else {
		for (; itLoc != iteLoc; itLoc++) {
			Location* location = itLoc->second;
			location->applyDefaultValues(defaultLocation);
		}
	}
}

Response*	Server::handleError(int statusCode, Location *location) const {
	std::map<int, std::string> const& errorPagesMap = location->getErrorPages();
	Response *res = new Response();

	if (errorPagesMap.find(statusCode) != errorPagesMap.end()) {
		File file(errorPagesMap.find(statusCode)->second);
		res->setStatusCode(statusCode);
		res->setMimeByExtension(file.getExtension());
		res->setBody(file.getFileContent());
	} else {
		res->setStatusCode(statusCode);
		res->setSingleHeader("Content-Type", "text/plain");
		res->setBody("[Error]" + res->getStatusCode() + " : " + res->getStatusMessage());
	}
	return res;
}

std::string	Server::setFileListHtmlToReqBody(std::map<std::string, std::string> const& filesList,
										Request const& req) const {

	std::string fileListHTML = "<!DOCTYPE html><html>\n<head>\n<title>"
								+ req.getUri()
								+ " File Listing</title>\n</head>\n<body>\n<table>\n";

	std::map<std::string, std::string>::const_iterator it = filesList.begin();
	std::map<std::string, std::string>::const_iterator ite = filesList.end();
	for (; it != ite; it++) {
		if (it->first == "../" && req.getUri() == "/") {
			continue;
		}
		fileListHTML += "<tr><td><a href=\"" + it->first + "\">" + it->first
						+ "</a></td> <td>&emsp;&emsp;(" + it->second + " bytes)</td></tr>\n";
	}
	fileListHTML += "</table>\n</body>\n</html>\n";
	return fileListHTML;
}

Response*	Server::handleIndexing(File const& file, Request const& req, Location *location) const {

	std::vector<std::string>::const_iterator it = location->getIndex().begin();
	std::vector<std::string>::const_iterator ite = location->getIndex().end();
	for (; it != ite; it++) {
		File index(file.getFullPath() + "/" + *it);
		if (index.isReadable()) {
			std::string const& indexContent = index.getFileContent();
			Response *res = new Response();
			res->setStatusCode(200);
			res->setMimeByExtension(index.getExtension());
			res->setBody(indexContent);
			return res;
		}
	}

	if (location->getAutoIndex()) {
		Response *res = new Response();
		res->setStatusCode(200);
		res->setBody(setFileListHtmlToReqBody(file.getFilesListing(), req));
		return res;
	}
	return handleError(404, location);
}

Response*	Server::handleGetRequest(Request const& req, Location *location) const {
	std::string const& path = location->getRoot() + req.getUri();

	File file(path);
	if (file.exists() == false) {
		return this->handleError(404, location);
	}

	try {
		if (file.isDirectory() == true) {
			return this->handleIndexing(file, req, location);
		}

		std::string const& fileContent = file.getFileContent();
		Response *res = new Response();
		res->setMimeByExtension(file.getExtension());
		res->setBody(fileContent);
		res->setStatusCode(200);
		return res;
	} catch (std::exception& e) {
		return handleError(404, location);
	}
}

Response*	Server::handlePostRequest(Request const& req, Location *location) const {
	(void)req;
	(void)location;
	return NULL;
}

Response*	Server::handleDeleteRequest(Request const& req, Location *location) const {
	(void)req;
	(void)location;

	return NULL;
}

Response*	Server::handleClientRequest(Request const& req) const {

	std::cout << req.getStartLine() << std::endl;
	if (req.isRequestValid() == false) {
		return this->handleError(400, this->locationsMap_.find("/")->second);
	}

	std::string const& method = req.getMethod();
	Location *location = getCorrespondingLocation(req.getUri());
	if (method == "GET") {
		return this->handleGetRequest(req, location);
	} else if (method == "POST") {
		return this->handlePostRequest(req, location);
	} else if (method == "DELETE") {
		return this->handleDeleteRequest(req, location);
	}
	return this->handleError(501, this->locationsMap_.find("/")->second);
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
