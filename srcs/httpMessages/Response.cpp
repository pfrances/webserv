/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:21 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/18 16:06:07 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ParseTools.hpp"
#include <sstream>
#include <iostream>

Response::Response(void) :	HttpMessage(),
							statusCode_(""),
							statusMessage_(""),
							httpVersion_("HTTP/1.1"),
							cgiHandler_(NULL) {
	this->setSingleHeader("Content-Type", "text/plain");
}

Response::Response(std::string const& rawResponse) :	HttpMessage(rawResponse),
														statusCode_(""),
														statusMessage_(""),
														httpVersion_(""),
														cgiHandler_(NULL) {
	try {
		parseStartLine();
	} catch (std::exception& e) {
		isValid_ = false;
	}
}

Response::~Response(void) {

}

Response::Response(Response const& other) : HttpMessage(other),
											statusCode_(other.statusCode_),
											statusMessage_(other.statusMessage_),
											httpVersion_(other.httpVersion_),
											cgiHandler_(NULL) {

}

Response &Response::operator=(Response const& other) {
	if (this != &other) {
		HttpMessage::operator=(other);
		this->statusCode_ = other.statusCode_;
		this->statusMessage_ = other.statusMessage_;
		this->httpVersion_ = other.httpVersion_;
		if (this->cgiHandler_)
			delete this->cgiHandler_;
		this->cgiHandler_ = other.cgiHandler_;
	}
	return (*this);
}

std::string const&	Response::getStatusCode(void) const {
	return (this->statusCode_);
}

std::string const&	Response::getStatusMessage(void) const {
	return (this->statusMessage_);
}

std::string const&	Response::getHttpVersion(void) const {
	return (this->httpVersion_);
}

bool	Response::hasCgiHandler(void) const {
	return (this->cgiHandler_ != NULL);
}

CgiHandler*	Response::getCgiHandler(void) {
	return this->cgiHandler_;
}

void	Response::setStatusCode(std::string const& statusCode) {
	this->statusCode_ = statusCode;
	setStatusMessageFromCode(ParseTools::stringToInt(statusCode));
	this->updateStartLine();
	this->updateRawMessage();
}

void	Response::setStatusCode(int statusCode) {
	this->statusCode_ = ParseTools::intToString(statusCode);
	setStatusMessageFromCode(statusCode);
	this->updateStartLine();
	this->updateRawMessage();
}

void	Response::setStatusMessageFromCode(int statusCode) {

	if (statusCode == 100) {
		this->statusMessage_ = "Continue";
	} else if (statusCode == 101) {
		this->statusMessage_ = "Switching Protocols";
	} else if (statusCode == 102) {
		this->statusMessage_ = "Processing";
	} else if (statusCode == 200) {
		this->statusMessage_ = "OK";
	} else if (statusCode == 201) {
		this->statusMessage_ = "Created";
	} else if (statusCode == 202) {
		this->statusMessage_ = "Accepted";
	} else if (statusCode == 204) {
		this->statusMessage_ = "No Content";
	} else if (statusCode == 206) {
		this->statusMessage_ = "Partial Content";
	} else if (statusCode == 300) {
		this->statusMessage_ = "Multiple Choices";
	} else if (statusCode == 301) {
		this->statusMessage_ = "Moved Permanently";
	} else if (statusCode == 302) {
		this->statusMessage_ = "Found";
	} else if (statusCode == 303) {
		this->statusMessage_ = "See Other";
	} else if (statusCode == 304) {
		this->statusMessage_ = "Not Modified";
	} else if (statusCode == 307) {
		this->statusMessage_ = "Temporary Redirect";
	} else if (statusCode == 308) {
		this->statusMessage_ = "Permanent Redirect";
	} else if (statusCode == 400) {
		this->statusMessage_ = "Bad Request";
	} else if (statusCode == 401) {
		this->statusMessage_ = "Unauthorized";
	} else if (statusCode == 403) {
		this->statusMessage_ = "Forbidden";
	} else if (statusCode == 404) {
		this->statusMessage_ = "Not Found";
	} else if (statusCode == 405) {
		this->statusMessage_ = "Method Not Allowed";
	} else if (statusCode == 408) {
		this->statusMessage_ = "Request Timeout";
	} else if (statusCode == 409) {
		this->statusMessage_ = "Conflict";
	} else if (statusCode == 410) {
		this->statusMessage_ = "Gone";
	} else if (statusCode == 413) {
		this->statusMessage_ = "Payload Too Large";
	} else if (statusCode == 415) {
		this->statusMessage_ = "Unsupported Media Type";
	} else if (statusCode == 422) {
		this->statusMessage_ = "Unprocessable Entity";
	} else if (statusCode == 429) {
		this->statusMessage_ = "Too Many Requests";
	} else if (statusCode == 500) {
		this->statusMessage_ = "Internal Server Error";
	} else if (statusCode == 501) {
		this->statusMessage_ = "Not Implemented";
	} else if (statusCode == 502) {
		this->statusMessage_ = "Bad Gateway";
	} else if (statusCode == 503) {
		this->statusMessage_ = "Service Unavailable";
	} else if (statusCode == 504) {
		this->statusMessage_ = "Gateway Timeout";
	} else if (statusCode == 505) {
		this->statusMessage_ = "HTTP Version Not Supported";
	} else {
		this->statusMessage_ = "error_" + ParseTools::intToString(statusCode);
	}
}

void	Response::setStatusMessage(std::string const& statusMessage) {
	this->statusMessage_ = statusMessage;
	this->updateStartLine();
	this->updateRawMessage();
}

void	Response::setHttpVersion(std::string const& httpVersion) {
	this->httpVersion_ = httpVersion;
	this->updateStartLine();
	this->updateRawMessage();
}

void	Response::setCgiHandler(std::string const& path, std::string const& cgiExecutor, Request const& request) {
	if (this->cgiHandler_ != NULL) {
		delete this->cgiHandler_;
	}
	this->cgiHandler_ = new CgiHandler(path, cgiExecutor);
	this->cgiHandler_->setEnv(request);
	try {
		this->cgiHandler_->executeCgi();
	} catch (std::exception& e) {
		delete this->cgiHandler_;
		this->cgiHandler_ = NULL;
		throw std::runtime_error("CGI execution failed");
	}
}

void	Response::parseStartLine(void) {
	std::istringstream iss(this->getStartLine());
	std::string tmp;

	std::getline(iss, tmp, ' ');
	if (tmp == "Status:") {
		this->setHttpVersion("HTTP/1.1");
	} else {
		this->setHttpVersion(tmp);
	}

	std::getline(iss, tmp, ' ');
	this->setStatusCode(tmp);

	std::getline(iss, tmp);
	this->setStatusMessage(tmp);
}

void	Response::updateStartLine(void) {
	this->setStartLine(this->httpVersion_ + " " + this->statusCode_ + " " + this->statusMessage_);
}

bool	Response::isHttpVersionValid(void) const {
	return (this->httpVersion_ == "HTTP/1.1");
}

bool	Response::isStatusCodeValid(void) const {
	try {
		int statusCode = ParseTools::stringToInt(this->statusCode_);
		return (statusCode >= 100 && statusCode <= 599);
	} catch (std::exception& e) {
		return (false);
	}
}

bool	Response::isValid(void) const {
	return (HttpMessage::isValid() && isHttpVersionValid() && isStatusCodeValid());
}
