/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:21 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:39:42 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) :	HttpMessage(),
							statusCode_(""),
							statusMessage_(""),
							httpVersion_("") {

}

Response::Response(std::string rawResponse) :	HttpMessage(rawResponse),
												statusCode_(""),
												statusMessage_(""),
												httpVersion_("") {
	parseStartLine();
}

Response::~Response(void) {

}

Response::Response(const Response &other) : HttpMessage(other),
											statusCode_(other.statusCode_),
											statusMessage_(other.statusMessage_),
											httpVersion_(other.httpVersion_) {

}

Response &Response::operator=(const Response &other) {
	if (this != &other) {
		HttpMessage::operator=(other);
		this->statusCode_ = other.statusCode_;
		this->statusMessage_ = other.statusMessage_;
		this->httpVersion_ = other.httpVersion_;
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

void	Response::setStatusCode(std::string const& statusCode) {
	this->statusCode_ = statusCode;
}

void	Response::setStatusMessage(std::string const& statusMessage) {
	this->statusMessage_ = statusMessage;
}

void	Response::setHttpVersion(std::string const& httpVersion) {
	this->httpVersion_ = httpVersion;
}

void	Response::parseStartLine(void) {
	std::string		line;
	std::string		tmp;
	std::string		startLine;

	startLine = this->getStartLine();
	line = startLine;
	tmp = line.substr(0, line.find(" "));
	this->setHttpVersion(tmp);
	line = line.substr(line.find(" ") + 1);
	tmp = line.substr(0, line.find(" "));
	this->setStatusCode(tmp);
	line = line.substr(line.find(" ") + 1);
	this->setStatusMessage(line);
}

void	Response::updateStartLine(void) {
	this->setStartLine(this->httpVersion_ + " " + this->statusCode_ + " " + this->statusMessage_);
}
