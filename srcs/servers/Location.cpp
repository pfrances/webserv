/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:23:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/24 13:38:08 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "ParseTools.hpp"
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <iostream>

Location::Location(void) :	path_(""),
							root_(""),
							index_(std::vector<std::string>()),
							tryFiles_(std::vector<std::string>()),
							errorPages_(std::map<int, std::string>()),
							redirect_(std::map<std::string, std::string>()),
							allowedMethods_(std::vector<std::string>()),
							cgiPath_(std::vector<std::string>()),
							cgiExtension_(std::vector<std::string>()),
							clientMaxBodySize_(0),
							directoryListing_(false) {

}

Location::Location(std::string const& locationConf,
					std::string const& path) :	path_(path),
												root_(""),
												index_(std::vector<std::string>()),
												tryFiles_(std::vector<std::string>()),
												errorPages_(std::map<int, std::string>()),
												redirect_(std::map<std::string, std::string>()),
												allowedMethods_(std::vector<std::string>()),
												cgiPath_(std::vector<std::string>()),
												cgiExtension_(std::vector<std::string>()),
												clientMaxBodySize_(0),
												directoryListing_(false) {
	this->parseLocationConf(locationConf);
}

Location::Location(Location const& other) :	path_(other.path_),
											root_(other.root_),
											index_(other.index_),
											tryFiles_(other.tryFiles_),
											errorPages_(other.errorPages_),
											redirect_(other.redirect_),
											allowedMethods_(other.allowedMethods_),
											cgiPath_(other.cgiPath_),
											cgiExtension_(other.cgiExtension_),
											clientMaxBodySize_(other.clientMaxBodySize_),
											directoryListing_(other.directoryListing_) {

}

Location &Location::operator=(Location const& other) {
	if (this != &other) {
		this->path_ = other.path_;
		this->root_ = other.root_;
		this->index_ = other.index_;
		this->tryFiles_ = other.tryFiles_;
		this->errorPages_ = other.errorPages_;
		this->redirect_ = other.redirect_;
		this->allowedMethods_ = other.allowedMethods_;
		this->cgiPath_ = other.cgiPath_;
		this->cgiExtension_ = other.cgiExtension_;
		this->clientMaxBodySize_ = other.clientMaxBodySize_;
		this->directoryListing_ = other.directoryListing_;
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

std::vector<std::string> const& Location::getTryFiles(void) const {
	return (this->tryFiles_);
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

std::vector<std::string> const& Location::getCgiPath(void) const {
	return (this->cgiPath_);
}

std::vector<std::string> const& Location::getCgiExtension(void) const {
	return (this->cgiExtension_);
}

size_t Location::getClientMaxBodySize(void) const {
	return (this->clientMaxBodySize_);
}

bool Location::getDirectoryListing(void) const {
	return (this->directoryListing_);
}

void Location::setPath(std::string const& path) {
	this->path_ = path;
}

void Location::setRoot(std::string const& root) {
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

void Location::setTryFiles(std::vector<std::string> const& tryFiles) {
	this->tryFiles_ = tryFiles;
}

void Location::addTryFiles(std::string const& tryFiles) {
	this->tryFiles_.push_back(tryFiles);
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
				throw std::runtime_error("Error: invalid error code");
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

void Location::setCgiPath(std::vector<std::string> const& cgiPath) {
	this->cgiPath_ = cgiPath;
}

void Location::addCgiPath(std::string const& cgiPath) {
	this->cgiPath_.push_back(cgiPath);
}

void Location::setCgiExtension(std::vector<std::string> const& cgiExtension) {
	this->cgiExtension_ = cgiExtension;
}

void Location::addCgiExtension(std::string const& cgiExtension) {
	this->cgiExtension_.push_back(cgiExtension);
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
			throw std::runtime_error("Error: client_max_body_size is too big -- overflow");
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
				throw std::runtime_error("Error: client_max_body_size is too big -- overflow");
			break;
		default:
			throw std::runtime_error("Error: client_max_body_size is invalid");
	}
	it++;
	while (std::isspace(*it)) {
		it++;
	}
	if (it != ite) {
		throw std::runtime_error("Error: client_max_body_size is invalid");
	}
}

void Location::setDirectoryListing(bool directoryListing) {
	this->directoryListing_ = directoryListing;
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
				throw std::runtime_error("Location [" + this->path_ + "]: root: no semicolon.");
			}
		} else if (token == "index") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setIndex(tokensVector);
		} else if (token == "try_files") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setTryFiles(tokensVector);
		} else if (token == "error_page") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->parseAndAddErrorPages(tokensVector);
		} else if (token == "redirect") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			if (tokensVector.size() != 2) {
				throw std::runtime_error("Location [" + this->path_ + "]: redirect: invalid number of arguments.");
			}
			this->addRedirect(tokensVector[0], tokensVector[1]);
		} else if (token == "methods") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setAllowedMethods(tokensVector);
		} else if (token == "cgi_path") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiPath(tokensVector);
		} else if (token == "cgi_extension") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExtension(tokensVector);
		} else if (token == "client_max_body_size") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setClientMaxBodySize(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw std::runtime_error("Location [" + this->path_ + "]: client_max_body_size: no semicolon.");
			}
		} else if (token == "upload_path") {
			token = ParseTools::getNextToken(locationBlock, it);
			this->setUploadPath(token);
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw std::runtime_error("Location [" + this->path_ + "]: upload_path: no semicolon.");
			}
		} else if (token == "execute_cgi") {
			tokensVector = ParseTools::getAllTokensUntilSemicolon(locationBlock, it);
			this->setCgiExtension(tokensVector);
		} else if (token == "directory_listing") {
			token = ParseTools::getNextToken(locationBlock, it);
			if (token == "on") {
				this->setDirectoryListing(true);
			} else if (token == "off") {
				this->setDirectoryListing(false);
			} else {
				throw std::runtime_error("Location [" + this->path_ + "]: directory_listing: invalid argument.");
			}
			if (ParseTools::getNextToken(locationBlock, it) != ";") {
				throw std::runtime_error("Location [" + this->path_ + "]: directory_listing: no semicolon.");
			}
		} else {
			throw std::runtime_error("Location [" + this->path_ + "]: unknown directive: '" + token + "'.");
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
	if (this->tryFiles_.empty()) {
		this->tryFiles_ = defaultLocation.tryFiles_;
	}
	if (this->errorPages_.empty()) {
		this->errorPages_ = defaultLocation.errorPages_;
	}
	if (this->redirect_.empty()) {
		this->redirect_ = defaultLocation.redirect_;
	}
	if (this->allowedMethods_.empty()) {
		this->allowedMethods_ = defaultLocation.allowedMethods_;
	}
	if (this->cgiPath_.empty()) {
		this->cgiPath_ = defaultLocation.cgiPath_;
	}
	if (this->cgiExtension_.empty()) {
		this->cgiExtension_ = defaultLocation.cgiExtension_;
	}
	if (this->clientMaxBodySize_ == 0) {
		this->clientMaxBodySize_ = defaultLocation.clientMaxBodySize_;
	}
	if (this->uploadPath_.empty()) {
		this->uploadPath_ = defaultLocation.uploadPath_;
	}
	if (this->directoryListing_ == false) {
		this->directoryListing_ = defaultLocation.directoryListing_;
	}
}
