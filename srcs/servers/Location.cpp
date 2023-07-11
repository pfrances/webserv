/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:23:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/10 17:02:29 by pfrances         ###   ########.fr       */
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
							index_(std::vector<std::string>()),
							errorPages_(std::map<int, std::string>()),
							redirect_(std::map<std::string, std::string>()),
							allowedMethods_(std::vector<std::string>()),
							cgiExecutor_(std::vector<std::string>()),
							cgiExtensions_(std::vector<std::string>()),
							clientMaxBodySize_(1024),
							autoIndex_(false) {
	allowedMethods_.push_back("GET");
}

Location::Location(std::string const& locationConf,
					std::string const& path) :	path_(path),
												root_(""),
												uploadPath_("./"),
												index_(std::vector<std::string>()),
												errorPages_(std::map<int, std::string>()),
												redirect_(std::map<std::string, std::string>()),
												allowedMethods_(std::vector<std::string>()),
												cgiExecutor_(std::vector<std::string>()),
												cgiExtensions_(std::vector<std::string>()),
												clientMaxBodySize_(1024),
												autoIndex_(false) {
	this->parseLocationConf(locationConf);
}

Location::Location(Location const& other) :	path_(other.path_),
											root_(other.root_),
											uploadPath_(other.uploadPath_),
											index_(other.index_),
											errorPages_(other.errorPages_),
											redirect_(other.redirect_),
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
		this->redirect_ = other.redirect_;
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

std::map<std::string, std::string> const& Location::getRedirect(void) const {
	return (this->redirect_);
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

void Location::setRedirect(std::map<std::string, std::string> const& redirect) {
	this->redirect_ = redirect;
}

void Location::addRedirect(std::string const& redirect, std::string const& redirectPath) {
	this->redirect_.insert(std::pair<std::string, std::string>(redirect, redirectPath));
}

void Location::setAllowedMethods(std::vector<std::string> const& allowedMethods) {
	this->allowedMethods_ = allowedMethods;
}

void Location::addAllowedMethods(std::string const& allowedMethods) {
	this->allowedMethods_.push_back(allowedMethods);
}

void Location::setCgiExecutor(std::vector<std::string> const& cgiExecutor) {
	this->cgiExecutor_ = cgiExecutor;
}

void Location::addCgiExecutor(std::string const& cgiExecutor) {
	this->cgiExecutor_.push_back(cgiExecutor);
}

void Location::setCgiExtension(std::vector<std::string> const& cgiExtension) {
	this->cgiExtensions_ = cgiExtension;
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

	std::vector<std::string> tokensVector;
	std::string token = ParseTools::getNextToken(locationBlock, it);
	while (token.empty() == false) {
		if (token == "root") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setRoot(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw ConfigurationException("Location [" + this->path_ + "]: root: no semicolon.");
			}
		} else if (token == "index") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setIndex(tokensVector);
		} else if (token == "error_page") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->parseAndAddErrorPages(tokensVector);
		} else if (token == "redirect") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			if (tokensVector.size() != 2) {
				throw ConfigurationException("Location [" + this->path_ + "]: redirect: invalid number of arguments.");
			}
			this->addRedirect(tokensVector[0], tokensVector[1]);
		} else if (token == "allow_methods") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setAllowedMethods(tokensVector);
		} else if (token == "cgi_executor") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExecutor(tokensVector);
		} else if (token == "cgi_ext") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExtension(tokensVector);
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
		} else if (token == "autoindex") {
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
	if (this->redirect_.empty()) {
		this->redirect_ = defaultLocation.redirect_;
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
	if (this->clientMaxBodySize_ == 0) {
		this->clientMaxBodySize_ = defaultLocation.clientMaxBodySize_;
	}
	if (this->uploadPath_.empty()) {
		this->uploadPath_ = defaultLocation.uploadPath_;
	}
	if (this->autoIndex_ == false) {
		this->autoIndex_ = defaultLocation.autoIndex_;
	}
}
