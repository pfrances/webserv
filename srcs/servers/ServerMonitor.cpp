/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 20:27:30 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ParseTools.hpp"
#include "File.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>

ServerMonitor::ServerMonitor(std::string const& confFileName) :	serversMap_(),
																clientsMap_(),
																cgiHandlersMap_(),
																responsesMap_(),
																pollfdsVec_(),
																timer_(),
																logsOn_(false),
																cookiesMap_() {
	this->addNewPollfd(STDIN_FILENO, POLLIN);
	try {
		this->parseConfigFile(confFileName);
	} catch (std::exception& e) {
		for (std::map<int, Server*>::iterator it = serversMap_.begin(); it != serversMap_.end(); it++) {
			delete it->second;
			it->second = NULL;
		}
		throw std::runtime_error(e.what());
	}
}

ServerMonitor::ServerMonitor(ServerMonitor const& other) :	serversMap_(other.serversMap_),
															clientsMap_(other.clientsMap_),
															cgiHandlersMap_(other.cgiHandlersMap_),
															responsesMap_(other.responsesMap_),
															pollfdsVec_(other.pollfdsVec_),
															timer_(other.timer_),
															logsOn_(other.logsOn_),
															cookiesMap_(other.cookiesMap_) {

}

ServerMonitor &ServerMonitor::operator=(ServerMonitor const& other) {
	if (this != &other) {
		this->serversMap_ = other.serversMap_;
		this->clientsMap_ = other.clientsMap_;
		this->responsesMap_ = other.responsesMap_;
		this->cgiHandlersMap_ = other.cgiHandlersMap_;
		this->pollfdsVec_ = other.pollfdsVec_;
		this->timer_ = other.timer_;
		this->logsOn_ = other.logsOn_;
		this->cookiesMap_ = other.cookiesMap_;
	}
	return (*this);
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
		itServ->second = NULL;
	}

	std::map<int, Response*>::iterator itRes = responsesMap_.begin();
	std::map<int, Response*>::iterator iteRes = responsesMap_.end();
	for(; itRes != iteRes; itRes++) {
		delete itRes->second;
		itRes->second = NULL;
	}

	std::map<int, CgiHandler*>::iterator itCgi = cgiHandlersMap_.begin();
	std::map<int, CgiHandler*>::iterator iteCgi = cgiHandlersMap_.end();
	for(; itCgi != iteCgi; itCgi++) {
		delete itCgi->second;
		itCgi->second = NULL;
	}

	std::map<std::string, Cookie*>::iterator itCookie = this->cookiesMap_.begin();
	std::map<std::string, Cookie*>::iterator iteCookie = this->cookiesMap_.end();
	for (; itCookie != iteCookie; itCookie++) {
		delete itCookie->second;
		itCookie->second = NULL;
	}
}

void	ServerMonitor::parseConfigFile(std::string const& configFileName){
	File				confFile(configFileName);
	std::string const&	conf = confFile.getFileContent();

	std::string::const_iterator	it = conf.begin();
	std::string token = ParseTools::getNextToken(conf, it);
	while (token.empty() == false)
	{
		if (token == "server") {
			Server *server = NULL;
			std::string serverBlock = ParseTools::extractBlock(conf, it);
			try {
				server = new Server(serverBlock);
			} catch (std::exception& e) {
				delete server;
				throw std::runtime_error(e.what());
			}
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
		server->startListen(LISTEN_BACKLOG);
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
	Server *server = this->serversMap_[fd];
	this->clientsMap_.insert(std::pair<int, Server*>(clientfd, server));
	server->setClientLastRequestTime(clientfd, this->timer_.getElapsedTimeSince());
}

int	ServerMonitor::getPollfdsVecIndexFromFd(int fd) const {
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
	} else if (msg == "logs" || msg == "log") {
		this->switchLogs();
	} else if (msg == "cookies" || msg == "cookie") {
		for (std::map<std::string, Cookie*>::iterator it = this->cookiesMap_.begin(); it != this->cookiesMap_.end(); it++) {
			std::cout << it->first << std::endl;
		}
		if (this->cookiesMap_.size() == 0) {
			std::cout << "No cookies" << std::endl;
		}
	} else if (msg == "reset cookies" || msg == "reset cookie") {
		for (std::map<std::string, Cookie*>::iterator it = this->cookiesMap_.begin(); it != this->cookiesMap_.end(); it++) {
			delete it->second;
		}
		this->cookiesMap_.clear();
	} else if (msg.find("rm ") == 0) {
		std::string cookie = msg.substr(3);
		if (this->cookiesMap_.find(cookie) != this->cookiesMap_.end()) {
			delete this->cookiesMap_[cookie];
			this->cookiesMap_.erase(cookie);
		} else {
			std::cout << "Unknown cookie: " << cookie << std::endl;
		}
	}
	else if (msg.find("show ") == 0) {
		std::string cookie = msg.substr(5);
		if (this->cookiesMap_.find(cookie) != this->cookiesMap_.end()) {
			std::cout << "Cookie: " << cookie << "\n" << this->cookiesMap_[cookie]->getContent() << std::endl;
		} else {
			std::cout << "Unknown cookie: " << cookie << std::endl;
		}
	} else if (msg == "session" || msg == "sessions") {
		std::map<int, Server*>::iterator it = this->clientsMap_.begin();
		std::map<int, Server*>::iterator ite = this->clientsMap_.end();
		for (; it != ite; it++) {
			std::time_t lastRequestTime = it->second->getClientLastRequestTime(it->first);
			std::cout << "Fd: [" << it->first << "]\t Last request time\t";
			std::cout << std::asctime(std::localtime(&lastRequestTime));
		}
		if (this->clientsMap_.size() == 0) {
			std::cout << "No active sessions" << std::endl;
		}
	} else if (msg.find("close ") == 0) {
		try {
			int fd = ParseTools::stringToInt(msg.substr(6));
			if (this->clientsMap_.find(fd) != this->clientsMap_.end()) {
				closeConnection(fd);
				std::cout << "Connection closed: " << fd << std::endl;
			} else {
				std::cout << "Unknown client: " << fd << std::endl;
			}
		} catch (std::exception& e) {
			std::cout << "Invalid file descriptor: " << msg.substr(6) << std::endl;
			return ;
		}
	} else if (msg == "help") {
		std::cout << "Available commands:" << std::endl;
		std::cout << "Enable/disable logs: [logs] or [log]" << std::endl;
		std::cout << "List all cookies: [cookies] or [cookie]" << std::endl;
		std::cout << "Reset cookies: [reset cookies] or [reset cookie]" << std::endl;
		std::cout << "Show a cookie: [show <cookie>]" << std::endl;
		std::cout << "Remove a cookie: [rm <cookie>]" << std::endl;
		std::cout << "List all sessions: [session] or [sessions]" << std::endl;
		std::cout << "Close a connection: [close <fd>]" << std::endl;
	} else if (!msg.empty()) {
		std::cout << "Unknown command: " << msg << std::endl;
	}
	if (!msg.empty()) {
		std::cout << std::endl;
	}
}

void	ServerMonitor::handleClientRequest(int fd) {
	std::string msg = this->recvMsg(fd);
	if (msg.empty()) {
		return ;
	}

	Request *req = new Request(msg);
	if (this->logsOn_) {
		this->timer_.printLogTime();
		std::cout << " REQUEST:\t" << req->getStartLine() << "\t(" << req->getHostName() << ")" << std::endl;
	}
	Server *server = this->clientsMap_[fd];
	Response *res = server->handleClientRequest(*req);
	this->monitorCookiesAtReception(req, res);
	if (res->hasCgiHandler()) {
		CgiHandler *cgi = res->getCgiHandler();
		cgi->setStartTime(this->timer_.getElapsedTimeSince());
		int cgiReadFd = cgi->getPipeReadFd();
		this->addNewPollfd(cgiReadFd, POLLIN);
		if (req->getMethod() != "GET" && !req->getBody().empty()) {
			int cgiWriteFd = cgi->getPipeWriteFd();
			this->addNewPollfd(cgiWriteFd, POLLOUT);
			this->cgiHandlersMap_[cgiWriteFd] = cgi;
		}
		this->cgiHandlersMap_[cgiReadFd] = cgi;
		cgi->setClientFd(fd);
		delete res;
		this->RemoveEventToPollfd(fd, POLLIN);
	} else {
		this->responsesMap_[fd] = res;
		this->setEventsToPollfd(fd, POLLOUT);
	}
	delete req;
	server->setClientLastRequestTime(fd, this->timer_.getElapsedTimeSince());
}

void	ServerMonitor::handleCgiResponse(pollfd const& pollfd) {

	std::string msg;
	if (pollfd.revents & POLLIN) {
		msg = this->readPipe(pollfd.fd);
	}

	CgiHandler *cgi = this->cgiHandlersMap_[pollfd.fd];
	std::string cookieId = cgi->getCookieId();
	bool		isCookieSet = cgi->isCookieIdSet();
	int clientFd = cgi->getClientFd();
	closeConnection(pollfd.fd);

	Response *res = new Response(msg);
	if (!res->isValid()) {
		delete res;
		Server *server = this->clientsMap_[clientFd];
		res = server->handleError(500);
	}
	if (this->cookiesMap_.find(cookieId) != this->cookiesMap_.end()) {
		this->cookiesMap_[cookieId]->addContent("RESPONSE:\t" + res->getStartLine() + "\n");
		if (isCookieSet) {
			res->setSingleHeader("Set-Cookie", cookieId);
		}
	}
	this->responsesMap_[clientFd] = res;
	this->setEventsToPollfd(clientFd, POLLOUT);
}

void	ServerMonitor::handleResponseToSend(int fd) {

	Response *res = this->responsesMap_[fd];
	if (res) {
		if (this->logsOn_) {
			this->timer_.printLogTime();
			std::cout << "RESPONSE:\t" << res->getStartLine() << std::endl;
		}
		std::string const& msg = res->getRawMessage();
		this->sendMsg(fd, msg);

		this->setEventsToPollfd(fd, POLLIN);
		delete res;
	}
	this->responsesMap_.erase(fd);
}

void	ServerMonitor::sendBodyToCgi(int fd) {
	CgiHandler *cgi = this->cgiHandlersMap_[fd];
	if (cgi) {
		cgi->writeBodyToCgiStdin();
	}
	removePollfd(fd);
	this->cgiHandlersMap_.erase(fd);
}

void	ServerMonitor::handleEvents(void) {
	for (size_t i = 0; i < pollfdsVec_.size(); i++) {
		try {
			if (pollfdsVec_[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
				if ((this->cgiHandlersMap_.find(pollfdsVec_[i].fd) != this->cgiHandlersMap_.end())) {
					this->handleCgiResponse(pollfdsVec_[i]);
				} else {
					this->closeConnection(pollfdsVec_[i].fd);
				}
			}
			if (pollfdsVec_[i].revents & POLLIN) {
				if (pollfdsVec_[i].fd == STDIN_FILENO) {
					this->handleUserInput();
				} else if (this->serversMap_.find(pollfdsVec_[i].fd) != this->serversMap_.end()) {
					this->handleNewConnection(pollfdsVec_[i].fd);
				} else if (this->clientsMap_.find(pollfdsVec_[i].fd) != this->clientsMap_.end()) {
					this->handleClientRequest(pollfdsVec_[i].fd);
				}
			}
			if (pollfdsVec_[i].revents & POLLOUT) {
				if (this->clientsMap_.find(pollfdsVec_[i].fd) != this->clientsMap_.end()) {
					this->handleResponseToSend(pollfdsVec_[i].fd);
				} else if (this->cgiHandlersMap_.find(pollfdsVec_[i].fd) != this->cgiHandlersMap_.end()) {
					this->sendBodyToCgi(pollfdsVec_[i].fd);
				}
			}
		} catch (IoTroubleException &e) {
			this->closeConnection(pollfdsVec_[i].fd);
		}
	}
}

void ServerMonitor::run() {
	this->setServersStartListen();
	while (1) {
		int ret = poll(pollfdsVec_.data(), pollfdsVec_.size(), POLL_TIMEOUT);
		if (ret < 0) {
			throw std::runtime_error("poll error");
		}
		if (ret > 0) {
			this->handleEvents();
		}
		this->checkTimeOut();
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
	std::vector<unsigned char> buffer(BUFFER_SIZE);
	std::vector<unsigned char> msg;

	while (true) {
		int bytesRead = recv(fd, buffer.data(), BUFFER_SIZE, 0);
		if (bytesRead < 0) {
			throw IoTroubleException("An issue occurred while receiving message");
		} else if (bytesRead == 0) {
			break;
		} else {
			msg.insert(msg.end(), buffer.begin(), buffer.begin() + bytesRead);
			if (bytesRead < BUFFER_SIZE) {
				break;
			}
		}
	}
	return std::string(msg.begin(), msg.end());
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
		if (it->fd == fd && fd > 0) {
			close(fd);
			it->fd = -1;
			it->events = 0;
			it->revents = 0;
		}
	}
}

void	ServerMonitor::addEventToPollfd(int fd, short event) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();

	for (; it != ite; it++) {
		if (it->fd == fd) {
			it->events |= event;
			break;
		}
	}
}

void	ServerMonitor::RemoveEventToPollfd(int fd, short event) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			it->events &= ~event;
			break;
		}
	}
}

void	ServerMonitor::setEventsToPollfd(int fd, short event) {
	std::vector<pollfd>::iterator it = this->pollfdsVec_.begin();
	std::vector<pollfd>::iterator ite = this->pollfdsVec_.end();
	for (; it != ite; it++) {
		if (it->fd == fd) {
			it->events = event;
			break;
		}
	}
}

void	ServerMonitor::checkTimeOut(void) {
	std::vector<int> fdsToClose;
	std::map<int, CgiHandler*>::iterator itCgi = this->cgiHandlersMap_.begin();
	std::map<int, CgiHandler*>::iterator iteCgi = this->cgiHandlersMap_.end();
	for (; itCgi != iteCgi; itCgi++) {
		CgiHandler* cgiHandler = itCgi->second;
		if (this->timer_.getElapsedTimeSince(cgiHandler->getStartTime()) > CGI_TIMEOUT) {
			int clientFd = cgiHandler->getClientFd();
			Server *server = this->clientsMap_[clientFd];
			Response *res = server->handleError(504);
			if (cgiHandler->getCookieId().empty() == false) {
				this->cookiesMap_[cgiHandler->getCookieId()]->addContent("RESPONSE:\t" + res->getStartLine() + "\n");
			}
			this->responsesMap_[clientFd] = res;
			this->setEventsToPollfd(clientFd, POLLOUT);
			fdsToClose.push_back(cgiHandler->getPipeReadFd());
			fdsToClose.push_back(cgiHandler->getPipeWriteFd());
		}
	}

	std::map<int, Server*>::iterator itClient = this->clientsMap_.begin();
	std::map<int, Server*>::iterator iteClient = this->clientsMap_.end();
	for (; itClient != iteClient; itClient++) {
		Server *server = itClient->second;
		int clientFd = itClient->first;
		if (this->timer_.getElapsedTimeSince(server->getClientLastRequestTime(clientFd)) > CLIENT_TIMEOUT) {
			server->removeClient(clientFd);
			fdsToClose.push_back(clientFd);
		}
	}

	for (size_t i = 0; i < fdsToClose.size(); i++) {
		this->closeConnection(fdsToClose[i]);
	}

	std::map<std::string, Cookie*>::iterator itCookie = this->cookiesMap_.begin();
	std::map<std::string, Cookie*>::iterator iteCookie = this->cookiesMap_.end();
	for (; itCookie != iteCookie; itCookie++) {
		Cookie *cookie = itCookie->second;
		if (cookie->isExpired()) {
			delete cookie;
			itCookie->second = NULL;
		}
	}
	for (itCookie = this->cookiesMap_.begin(); itCookie != iteCookie; itCookie++) {
		if (itCookie->second == NULL)
			this->cookiesMap_.erase(itCookie);
	}
}

void	ServerMonitor::switchLogs(void) {
	this->logsOn_ = !this->logsOn_;
	std::cout << "Switching logs " << (this->logsOn_ ? "on" : "off") << std::endl;
}

void	ServerMonitor::monitorCookiesAtReception(Request* req, Response* res) {

	std::string reqLog = " REQUEST:\t" + req->getStartLine() + "\t(" + req->getHostName() + ")\n";
	std::string resLog = "RESPONSE:\t" + res->getStartLine() + "\n";
	std::string cookieId = req->getCookieValue("id");
	Cookie *cookie;
	if (cookieId.empty() || this->cookiesMap_.find(cookieId) == this->cookiesMap_.end()) {
		cookie = new Cookie();
		cookieId = cookie->getCookieId();
		this->cookiesMap_[cookieId] = cookie;
		cookie->addContent(reqLog);
		if (res->hasCgiHandler()) {
			res->getCgiHandler()->setCookieId(cookieId);
			res->getCgiHandler()->setCookieIdIsNew(true);
		} else {
			res->setSingleHeader("Set-Cookie", "id=" + cookieId);
			cookie->addContent("RESPONSE:\t" + res->getStartLine() + "\n");
		}
	} else  {
		cookie = this->cookiesMap_[cookieId];
		cookie->addContent(reqLog);
		if (res->hasCgiHandler()) {
			res->getCgiHandler()->setCookieId(cookieId);
		}
	}
}
