/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:23:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 10:34:07 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "ParseTools.hpp"
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <iostream>

Location::Location(void) :	path_("/"),
							root_("./"),
							uploadPath_("./"),
							index_(),
							errorPages_(),
							redirectStatusCode_(-1),
							redirectUri_(""),
							allowedMethods_(),
							cgiExecutor_(),
							cgiExtensions_(),
							clientMaxBodySize_(8192),
							autoIndex_(false) {
	allowedMethods_.push_back("GET");
}

Location::Location(std::string const& locationConf,
					std::string const& path) :	path_(path),
												root_(""),
												uploadPath_("./"),
												index_(),
												errorPages_(),
												redirectStatusCode_(-1),
												redirectUri_(""),
												allowedMethods_(),
												cgiExecutor_(),
												cgiExtensions_(),
												clientMaxBodySize_(8192),
												autoIndex_(false) {
	this->parseLocationConf(locationConf);
	if (this->allowedMethods_.empty()) {
		this->allowedMethods_.push_back("GET");
	}
}

Location::Location(Location const& other) :	path_(other.path_),
											root_(other.root_),
											uploadPath_(other.uploadPath_),
											index_(other.index_),
											errorPages_(other.errorPages_),
											redirectStatusCode_(other.redirectStatusCode_),
											redirectUri_(other.redirectUri_),
											allowedMethods_(other.allowedMethods_),
											cgiExecutor_(other.cgiExecutor_),
											cgiExtensions_(other.cgiExtensions_),
											clientMaxBodySize_(other.clientMaxBodySize_),
											autoIndex_(other.autoIndex_) {

}

Location &Location::operator=(Location const& other) {
	if (this != &other) {
		this->path_ = other.path_;
		this->root_ = other.root_;
		this->index_ = other.index_;
		this->uploadPath_ = other.uploadPath_;
		this->errorPages_ = other.errorPages_;
		this->redirectUri_ = other.redirectUri_;
		this->redirectStatusCode_ = other.redirectStatusCode_;
		this->allowedMethods_ = other.allowedMethods_;
		this->cgiExecutor_ = other.cgiExecutor_;
		this->cgiExtensions_ = other.cgiExtensions_;
		this->clientMaxBodySize_ = other.clientMaxBodySize_;
		this->autoIndex_ = other.autoIndex_;
	}
	return (*this);
}

Location::~Location(void) {

}

std::string const& Location::getPath(void) const {
	return (this->path_);
}

std::string const& Location::getRoot(void) const {
	return (this->root_);
}

std::string const& Location::getUploadPath(void) const {
	return (this->uploadPath_);
}

std::vector<std::string> const& Location::getIndex(void) const {
	return (this->index_);
}

std::map<int, std::string> const& Location::getErrorPages(void) const {
	return (this->errorPages_);
}

std::string const&	Location::getRedirectUri(void) const {
	return (this->redirectUri_);
}

int	Location::getRedirectStatusCode(void) const {
	return (this->redirectStatusCode_);
}

std::vector<std::string> const& Location::getAllowedMethods(void) const {
	return (this->allowedMethods_);
}

std::vector<std::string> const& Location::getCgiExecutor(void) const {
	return (this->cgiExecutor_);
}

std::vector<std::string> const& Location::getCgiExtensions(void) const {
	return (this->cgiExtensions_);
}

std::string const&	Location::getCgiExecutorByExtension(std::string const& extension) const {
	std::vector<std::string> const& extensionVec =  this->getCgiExtensions();
	std::vector<std::string> const& executorVec =  this->getCgiExecutor();
	for (size_t i = 0; i < executorVec.size() && i < extensionVec.size(); i++) {
		if (extensionVec[i] == extension) {
			return executorVec[i];
		}
	}
	return executorVec.back();
}

size_t Location::getClientMaxBodySize(void) const {
	return (this->clientMaxBodySize_);
}

bool Location::getAutoIndex(void) const {
	return (this->autoIndex_);
}

bool	Location::isGetAllowed(void) const {
	std::vector<std::string>::const_iterator it = this->getAllowedMethods().begin();
	std::vector<std::string>::const_iterator ite = this->getAllowedMethods().end();
	for (; it != ite; it++) {
		if (*it == "GET") {
			return true;
		}
	}
	return false;
}

bool	Location::isPostAllowed(void) const {
	std::vector<std::string>::const_iterator it = this->getAllowedMethods().begin();
	std::vector<std::string>::const_iterator ite = this->getAllowedMethods().end();
	for (; it != ite; it++) {
		if (*it == "POST") {
			return true;
		}
	}
	return false;
}

bool	Location::isDeleteAllowed(void) const {
	std::vector<std::string>::const_iterator it = this->getAllowedMethods().begin();
	std::vector<std::string>::const_iterator ite = this->getAllowedMethods().end();
	for (; it != ite; it++) {
		if (*it == "DELETE") {
			return true;
		}
	}
	return false;
}

bool	Location::isCgiLocation(void) const {
	return !this->getCgiExecutor().empty() && !this->getCgiExtensions().empty();
}

void Location::setPath(std::string const& path) {
	this->path_ = path;
}

void Location::setRoot(std::string const& root) {
	if (*(root.end()) == '/')
		this->root_ = root.substr(0, root.size() - 1);
	else
		this->root_ = root;
}

void Location::setUploadPath(std::string const& uploadPath) {
	this->uploadPath_ = uploadPath;
}

void Location::setIndex(std::vector<std::string> const& index) {
	this->index_ = index;
}

void Location::addIndex(std::string const& index) {
	this->index_.push_back(index);
}

void Location::setErrorPages(std::map<int, std::string> const& errorPages) {
	this->errorPages_ = errorPages;
}

void Location::addErrorPages(int error, std::string const& errorPages) {
	this->errorPages_.insert(std::pair<int, std::string>(error, errorPages));
}

void Location::parseAndAddErrorPages(std::vector<std::string> const& errorPages) {

	std::vector<std::string>::const_iterator it = errorPages.begin();
	std::string pageName = errorPages.back();
	int error;

	for (; *it != pageName; ++it) {
		error = std::atoi((*it).c_str());
		if (error != 400 && error != 401 && error != 403 && error != 404 && error != 405
			&& error != 409 && error != 410 && error != 413 && error != 429
			&& error != 500 && error != 501 && error != 505) {
				throw ConfigurationException("[Configuration Location] " + this->getPath() + ": invalid error code");
			}
		this->addErrorPages(error, pageName);
	}
}

void Location::setRedirectUri(std::string const& uri) {
	if (uri.empty() || (uri.at(0) != '/' && uri.find("http://") != 0 && uri.find("https://") != 0)) {
		throw ConfigurationException("[Configuration Location] " + this->getPath() + ": invalid redirection path");
	}
	this->redirectUri_ = uri;
}

void	Location::setRedirectStatusCode(int statusCode) {
	if (statusCode != 300 && statusCode != 301 && statusCode != 302 && statusCode != 303
		&& statusCode != 304 && statusCode != 307 && statusCode != 308) {
			throw ConfigurationException("[Configuration Location] " + this->getPath() + ": invalid return code");
	}
	this->redirectStatusCode_ = statusCode;
}

void Location::setAllowedMethods(std::vector<std::string> const& allowedMethods) {
	this->allowedMethods_ = allowedMethods;
}

void Location::addAllowedMethods(std::string const& allowedMethods) {
	this->allowedMethods_.push_back(allowedMethods);
}

void Location::setCgiExecutor(std::vector<std::string> const& cgiExecutor) {
	this->cgiExecutor_ = cgiExecutor;
	this->cgiExecutor_.push_back("");
}

void Location::addCgiExecutor(std::string const& cgiExecutor) {
	this->cgiExecutor_.push_back(cgiExecutor);
}

void Location::setCgiExtension(std::vector<std::string> const& cgiExtension) {
	this->cgiExtensions_ = cgiExtension;
	this->cgiExtensions_.push_back("");
}

void Location::addCgiExtension(std::string const& cgiExtension) {
	this->cgiExtensions_.push_back(cgiExtension);
}

void Location::setClientMaxBodySize(size_t clientMaxBodySize) {
	this->clientMaxBodySize_ = clientMaxBodySize;
}

void Location::setClientMaxBodySize(std::string const& clientMaxBodySize) {
	if (clientMaxBodySize == "none") {
		this->clientMaxBodySize_ = 0;
		return ;
	}

	std::string::const_iterator it = clientMaxBodySize.begin();
	std::string::const_iterator ite = clientMaxBodySize.end();
	size_t	size = 0;

	while (std::isdigit(*it)) {
		char digit = *it - '0';
		if ((std::numeric_limits<size_t>::max() - digit) / 10 < size)
			throw ConfigurationException("[Configuration Location] " + this->getPath()
				+ ": client_body_size is too big");
		size = size * 10 + digit;
		++it;
	}

	while (std::isspace(*it)) {
		it++;
	}
	if (it == ite) {
		this->clientMaxBodySize_ = size;
		return ;
	}

	this->clientMaxBodySize_ = size;
	switch (*it) {
		case 'g':
		case 'G':
			this->clientMaxBodySize_ *= 1024;
			// Intentional fallthrough
		case 'm':
		case 'M':
			this->clientMaxBodySize_ *= 1024;
			// Intentional fallthrough
		case 'k':
		case 'K':
			this->clientMaxBodySize_ *= 1024;
			if (this->clientMaxBodySize_ < size)
			throw ConfigurationException("[Configuration Location] " + this->getPath()
				+ ": client_body_size is too big");
			break;
		default:
			throw ConfigurationException("[Configuration Location] " + this->getPath()
				+ ": invalid client_body_size");
	}
	it++;
	while (std::isspace(*it)) {
		it++;
	}
	if (it != ite) {
		throw ConfigurationException("[Configuration Location] " + this->getPath()
			+ ": invalid client_body_size");
	}
}

void Location::setAutoIndex(bool autoIndex) {
	this->autoIndex_ = autoIndex;
}

void Location::parseLocationConf(std::string const& locationBlock) {
	std::string::const_iterator	it = locationBlock.begin();

	std::vector<std::string> tokensVec;
	std::string token = ParseTools::getNextToken(locationBlock, it);
	while (token.empty() == false) {
		if (token == "root") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setRoot(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw ConfigurationException("Location [" + this->path_ + "]: root: no semicolon.");
			}
		} else if (token == "index") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setIndex(tokensVec);
		} else if (token == "error_page") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->parseAndAddErrorPages(tokensVec);
		} else if (token == "return") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			if (tokensVec.size() != 2) {
				throw ConfigurationException("Location [" + this->path_ + "]: return: invalid number of arguments.");
			}
			this->setRedirectStatusCode(ParseTools::stringToInt(tokensVec[0]));
			this->setRedirectUri(tokensVec[1]);
		} else if (token == "allow_methods") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setAllowedMethods(tokensVec);
		} else if (token == "cgi_executor") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExecutor(tokensVec);
		} else if (token == "cgi_ext") {
			tokensVec = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExtension(tokensVec);
		} else if (token == "client_max_body_size") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setClientMaxBodySize(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw ConfigurationException("Location [" + this->path_ + "]: client_max_body_size: no semicolon.");
			}
		} else if (token == "upload_path") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setUploadPath(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw ConfigurationException("Location [" + this->path_ + "]: upload_path: no semicolon.");
			}
		} else if (token == "auto_index") {
			token = ParseTools::getNextToken(locationBlock, it);
			if (token == "on") {
				this->setAutoIndex(true);
			} else if (token == "off") {
				this->setAutoIndex(false);
			} else {
				throw ConfigurationException("Location [" + this->path_ + "]: autoindex: invalid argument.");
			}
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw ConfigurationException("Location [" + this->path_ + "]: autoindex: no semicolon.");
			}
		} else {
			throw ConfigurationException("Location [" + this->path_ + "]: unknown directive: '" + token + "'.");
		}
		token = ParseTools::getNextToken(locationBlock, it);
	}
}

void	Location::applyDefaultValues(Location const& defaultLocation) {
	if (this->root_.empty()) {
		this->root_ = defaultLocation.root_;
	}
	if (this->index_.empty()) {
		this->index_ = defaultLocation.index_;
	}
	if (this->uploadPath_ == "./") {
		this->uploadPath_ = defaultLocation.uploadPath_;
	}
	if (this->errorPages_.empty()) {
		this->errorPages_ = defaultLocation.errorPages_;
	}
	if (this->redirectStatusCode_ < 0) {
		this->redirectStatusCode_ = defaultLocation.redirectStatusCode_;
	}
	if (this->redirectUri_.empty()) {
		this->redirectUri_ = defaultLocation.redirectUri_;
	}
	if (this->allowedMethods_.size() == 1 && this->allowedMethods_.front() == "GET") {
		this->allowedMethods_ = defaultLocation.allowedMethods_;
	}
	if (this->cgiExecutor_.empty()) {
		this->cgiExecutor_ = defaultLocation.cgiExecutor_;
	}
	if (this->cgiExtensions_.empty()) {
		this->cgiExtensions_ = defaultLocation.cgiExtensions_;
	}
	if (this->clientMaxBodySize_ == 8192) {
		this->clientMaxBodySize_ = defaultLocation.clientMaxBodySize_;
	}
	if (this->uploadPath_.empty()) {
		this->uploadPath_ = defaultLocation.uploadPath_;
	}
	if (this->autoIndex_ == false) {
		this->autoIndex_ = defaultLocation.autoIndex_;
	}
}
