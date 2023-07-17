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
#include "Location.hpp"
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
												locationsMap_(),
												subServMap_(),
												clientLastRequestTimeMap_() {
	this->parseServerConf(serverConf);
}

Server::Server(Server const& other) :	serverName_(other.serverName_),
										host_(other.host_),
										port_(other.port_),
										sockaddr_(other.sockaddr_),
										socketFd_(other.socketFd_),
										locationsMap_(other.locationsMap_),
										subServMap_(other.subServMap_),
										clientLastRequestTimeMap_(other.clientLastRequestTimeMap_) {

}

Server &Server::operator=(Server const& other) {
	if (this != &other) {
		this->serverName_ = other.serverName_;
		this->host_ = other.host_;
		this->port_ = other.port_;
		this->sockaddr_ = other.sockaddr_;
		this->locationsMap_ = other.locationsMap_;
		this->subServMap_ = other.subServMap_;
		this->clientLastRequestTimeMap_ = other.clientLastRequestTimeMap_;
	}
	return (*this);
}

Server::~Server(void) {
	std::map<std::string, Location*>::iterator itLoc = this->locationsMap_.begin();
	std::map<std::string, Location*>::iterator iteLoc = this->locationsMap_.end();
	for (; itLoc != iteLoc; itLoc++) {
		delete itLoc->second;
	}

	std::map<std::string, Server*>::iterator itSubServ = this->subServMap_.begin();
	std::map<std::string, Server*>::iterator iteSubServ = this->subServMap_.end();
	for (; itSubServ != iteSubServ; itSubServ++) {
		delete itSubServ->second;
	}
}

void	Server::prepareSocket(void) {
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

void	Server::addSubServer(Server *subServ) {
	std::string const& hostName = subServ->getServerName();

	if (hostName == this->serverName_ || this->subServMap_.find(hostName) != this->subServMap_.end()) {
		throw ConfigurationException("The Hostname '" + hostName + "' has been used twice for the same host:port.");
	}

	this->subServMap_[hostName] = subServ;
}

std::map<std::string, Location*> const&	Server::getLocationsMap(void) const {
	return (this->locationsMap_);
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

size_t Server::getClientLastRequestTime(int clientFd) const {
	if (this->clientLastRequestTimeMap_.find(clientFd) != this->clientLastRequestTimeMap_.end()) {
		return (this->clientLastRequestTimeMap_.at(clientFd));
	}
	return (0);
}

int	Server::getSocketFd(void) const {
	return (this->socketFd_);
}

void Server::setPort(std::string const& portStr) {
	int port = ParseTools::stringToInt(portStr);
	if (port < 0 || port > std::numeric_limits<unsigned short>::max()) {
		throw ConfigurationException("Server::setPort: invalid port");
	}
	this->port_ = static_cast<short>(port);
}

void	Server::setClientLastRequestTime(int clientFd, size_t time) {
	this->clientLastRequestTimeMap_[clientFd] = time;
}

void	Server::removeClient(int clientFd) {
	this->clientLastRequestTimeMap_.erase(clientFd);
}

void	Server::addLocation(Location* location) {
	this->locationsMap_.insert(std::pair<std::string, Location*>(location->getPath(), location));
}

int	Server::acceptNewClient(void) {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	return accept(this->socketFd_, (struct sockaddr *)&clientAddr, &clientAddrLen);
}

void	Server::startListen(int backlog) {
	if (listen(this->socketFd_, backlog) < 0) {
		throw std::runtime_error(this->getHost() + ":" + ParseTools::intToString(this->getPort()) + " listen error");
	}
}

void	Server::parseServerConf(std::string const& serverConf) {
	std::string::const_iterator	it = serverConf.begin();

	Location *defaultLocation = new Location();
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
			defaultLocation->parseAndAddErrorPages(tokensVec);
		} else if (token == "location") {
			token = ParseTools::getNextToken(serverConf, it);
			std::string locationBlock = ParseTools::extractBlock(serverConf, it);
			this->addLocation(new Location(locationBlock, token));
		} else if (token == "client_body_size") {
			token = ParseTools::getNextToken(serverConf, it);
			defaultLocation->setClientMaxBodySize(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw ConfigurationException("[Configuration Server] client_body_size: no semicolon.");
			}
		} else if (token == "index") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(serverConf, it);
			defaultLocation->setIndex(tokensVec);
		} else if (token == "return") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(serverConf, it);
			if (tokensVec.size() != 2) {
				throw ConfigurationException("Server [" + this->serverName_ + "]: return: invalid number of arguments.");
			}
			defaultLocation->setRedirectStatusCode(ParseTools::stringToInt(tokensVec[0]));
			defaultLocation->setRedirectUri(tokensVec[1]);
		} else if (token == "root") {
			token = ParseTools::getNextToken(serverConf, it);
			defaultLocation->setRoot(token);
			if (ParseTools::getNextToken(serverConf, it) != ";") {
				throw ConfigurationException("[Configuration Server] root: no semicolon.");
			}
		} else if (token == "autoindex") {
				token = ParseTools::getNextToken(serverConf, it);
			if (token == "on") {
				defaultLocation->setAutoIndex(true);
			} else if (token == "off") {
				defaultLocation->setAutoIndex(false);
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
	for (; itLoc != iteLoc; itLoc++) {
		Location* location = itLoc->second;
		location->applyDefaultValues(*defaultLocation);
	}
	if (this->locationsMap_.find("/") == iteLoc) {
		this->addLocation(defaultLocation);
	} else {
		delete defaultLocation;
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
		res->setBody("[Error " + res->getStatusCode() + "] " + res->getStatusMessage());
	}
	return res;
}

Response*	Server::handleError(int statusCode) const {
	return this->handleError(statusCode, this->locationsMap_.at("/"));
}

std::string	Server::setFileListHtmlToReqBody(std::map<std::string, std::string> const& filesList,
										Request const& req) const {

	std::string fileListHTML = "<!DOCTYPE html>\n<html>\n<head>\n<title>"
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
	File index;
	for (; it != ite; it++) {
		index = File(file.getFullPath() + "/" + *it);
		if (location->isCgiLocation() && index.exists()) {
			Response *res = new Response();
			res->setCgiHandler(index.getFullPath(), location->getCgiExecutorByExtension(index.getExtension()), req);
			return res;
		} else if (index.isReadable()) {
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
		res->setMimeByExtension("html");
		res->setBody(setFileListHtmlToReqBody(file.getFilesListing(), req));
		return res;
	}
	return handleError(404, location);
}

Response*	Server::handleGetRequest(Request const& req, Location *location) const {

	if (location->isGetAllowed() == false) {
		return handleError(405, location);
	}

	if (location->getRedirectStatusCode() > 0) {
		return (handleRedirection(req, location));
	}

	if (location->isCgiLocation()) {
		return this->handleCgiRequest(req, location);
	}

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

Response*	Server::handleLogPostRequest(Request const& req, Location *location) const {
	std::string const& body = req.getBody();
	if (body.length() > location->getClientMaxBodySize()) {
		return handleError(413, location);
	}

	std::string const& log_path = location->getUploadPath() + req.getUri().substr(location->getPath().length());
	std::map<std::string, std::string> queryMap = ParseTools::parseQuery(body);
	std::map<std::string, std::string>::const_iterator it = queryMap.begin();
	try {
		File file(log_path);
		std::string const& key = it->second;
		it++;
		std::string const& value = it->second;
		file.setKeyValue(key, value);
	} catch (UnvalidTokenException& e) {
		return handleError(422, location);
	} catch (std::exception& e) {
		return handleError(500, location);
	}
	if (location->getRedirectStatusCode() > 0) {
		return (handleRedirection(req, location));
	}
	Response *res = new Response();

	res->setStatusCode(201);
	std::string resBody =\
"<!DOCTYPE html>\n\
<html>\n\
	<head>\n\
		<title>Resource Created</title>\n\
	</head>\n\
	<body>\n\
 		<h1>Resource created successfully <a href=\"" + req.getUri() + "\">here/</a></h1>\n\
	</body>\n\
</html>";
	res->setMimeByExtension("html");
	res->setBody(resBody);
	return res;
}

Response*	Server::handleUploadPostRequest(Request const& req, Location *location) const {
	std::string const& body = req.getBody();
	if (body.length() > location->getClientMaxBodySize()) {
		return handleError(413, location);
	}

	std::string filePath = location->getUploadPath() + req.getUri().substr(location->getPath().length());
	File file(filePath);
	std::string fileExtension = file.getExtension().empty() ? "" : "." + file.getExtension();
	std::string countDuplicate;
	while (file.exists()) {
		filePath =  fileExtension.empty() ? filePath + "_" : filePath.substr(0, filePath.find(fileExtension)) + "_" + fileExtension;
		countDuplicate += "_";
		file = File(filePath);
	}
	try {
		file.setFileContent(ParseTools::parseBoundaryBody(body, req.getBoundary()));
	} catch (OpeningFailed& e) {
		return handleError(500, location);
	} catch (ConversionException& e) {
		return handleError(422, location);
	}
	Response *res;
	if (location->getRedirectStatusCode() > 0) {
		res = handleRedirection(req, location);
		std::string locationValue = res->getSingleHeader("Location");
		res->setSingleHeader("Location", locationValue.substr(0, locationValue.rfind(fileExtension)) + countDuplicate + fileExtension);
		return res;
	}
	res = new Response();
	res->setStatusCode(201);
	std::string resBody =\
"<!DOCTYPE html>\n\
<html>\n\
	<head>\n\
		<title>Resource Created</title>\n\
	</head>\n\
	<body>\n\
 		<h1>Resource created successfully</h1>\n\
	</body>\n\
</html>";
	res->setMimeByExtension("html");
	res->setBody(resBody);
	return res;
}

Response*	Server::handlePostRequest(Request const& req, Location *location) const {

	if (location->isPostAllowed() == false) {
		return handleError(405, location);
	}

	std::string const& contentType = req.getSingleHeader("Content-Type");
	if (contentType == "application/x-www-form-urlencoded") {
		return this->handleLogPostRequest(req, location);
	} else if (contentType.find("multipart/form-data") != std::string::npos)  {
		return this->handleUploadPostRequest(req, location);
	}
	return handleError(415, location);
}

Response*	Server::handleDeleteRequest(Request const& req, Location *location) const {

	if (location->isDeleteAllowed() == false) {
		return handleError(405, location);
	}

	std::string const& body = req.getBody();
	if (body.length() > location->getClientMaxBodySize()) {
		return handleError(413, location);
	}

	std::string const& log_path = location->getUploadPath() + req.getUri().substr(location->getPath().length());
	std::map<std::string, std::string>::const_iterator it = req.getQuery().begin();
	try {
		File file(log_path);
		file.unsetKey(it->second);
	} catch (UnvalidTokenException& e) {
		return handleError(422, location);
	} catch (std::exception& e) {
		return handleError(500, location);
	}

	if (location->getRedirectStatusCode() > 0) {
		return (handleRedirection(req, location));
	}

	Response *res = new Response();
	res->setStatusCode(204);
	std::string resBody =\
"<!DOCTYPE html>\n\
<html>\n\
	<head>\n\
		<title>Resource deleted</title>\n\
	</head>\n\
	<body>\n\
 		<h1>Resource deleted successfully</h1>\n\
	</body>\n\
</html>";
	res->setMimeByExtension("html");
	res->setBody(resBody);
	return res;
}

Response*	Server::handleCgiRequest(Request const& req, Location *location) const {

	std::string const& path = location->getRoot() + req.getUri();
	File file(path);
	if (file.isDirectory() == true) {
		return this->handleIndexing(file, req, location);
	}

	std::vector<std::string>::const_iterator it = location->getCgiExtensions().begin();
	std::vector<std::string>::const_iterator ite = location->getCgiExtensions().end();
	for (; it != ite; it++) {
		File cgi(path + *it);
		if (cgi.exists() == true) {
			Response *res = new Response();
			res->setCgiHandler(cgi.getFullPath(), location->getCgiExecutorByExtension(*it), req);
			return res;
		}
	}
	return handleError(404, location);
}

Response *Server::handleRedirection(Request const& req, Location *location) const {
	std::string uri = req.getUri().substr(location->getPath().length());
	if (!uri.empty() && uri.at(0) == '/') {
		uri = uri.substr(1);
	}

	std::string redirect = location->getRedirectUri();
	size_t pos = 0;
	while ((pos = redirect.find("$")) != std::string::npos) {
		redirect.replace(pos, redirect.length(), uri);
		pos++;
	}

	Response *res = new Response();
	res->setStatusCode(location->getRedirectStatusCode());
	res->setSingleHeader("Location", redirect);
	res->setBody("");
	return res;
}

Response*	Server::handleClientRequest(Request const& req) {

	std::cout << "Req:\t" << req.getStartLine() << "\t(" << req.getHostName() << ")" << std::endl;
	if (req.isValid() == false) {
		return this->handleError(400, this->locationsMap_.find("/")->second);
	}

	Location *location = getCorrespondingLocation(req);
	std::string const& method = req.getMethod();

	if (method == "GET") {
		return this->handleGetRequest(req, location);
	} else if (method == "POST") {
		return this->handlePostRequest(req, location);
	} else if (method == "DELETE") {
		return this->handleDeleteRequest(req, location);
	}
	return this->handleError(501, this->locationsMap_.find("/")->second);
}

Location*	Server::findLocation(std::map<std::string, Location*> const& locMap, std::string const& uri) {
	std::string tmp = uri;
	while (tmp.size() > 0)
	{
		std::map<std::string, Location*>::const_iterator it = locMap.find(tmp);
		if (it != locMap.end()) {
			return it->second;
		}
		size_t pos = tmp.find_last_of('/');
		if (pos == std::string::npos) {
			break;
		}
		tmp = tmp.substr(0, pos);
	}
	if (locMap.find("/") != locMap.end()) {
		return locMap.find("/")->second;
	}
	return NULL;
}

Location*	Server::getCorrespondingLocation(Request const& req) {

	std::string const& hostName = req.getHostName();
	if (this->subServMap_.find(hostName) != this->subServMap_.end()) {
		return findLocation(this->subServMap_[hostName]->getLocationsMap(), req.getUri());
	} else {
		return findLocation(this->getLocationsMap(), req.getUri());
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
