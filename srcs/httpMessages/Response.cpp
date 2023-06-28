/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:21 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 14:41:22 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sstream>

Response::Response(void) :	HttpMessage(),
							statusCode_(""),
							statusMessage_(""),
							httpVersion_("") {

}

Response::Response(std::string const& rawResponse) :	HttpMessage(rawResponse),
														statusCode_(""),
														statusMessage_(""),
														httpVersion_("") {
	parseStartLine();
}

Response::~Response(void) {

}

Response::Response(Response const& other) : HttpMessage(other),
											statusCode_(other.statusCode_),
											statusMessage_(other.statusMessage_),
											httpVersion_(other.httpVersion_) {

}

Response &Response::operator=(Response const& other) {
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
	this->updateStartLine();
	this->updateRawMessage();
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

void	Response::parseStartLine(void) {
	std::istringstream iss(this->getStartLine());
	std::string tmp;

	std::getline(iss, tmp, ' ');
	this->setHttpVersion(tmp);

	std::getline(iss, tmp, ' ');
	this->setStatusCode(tmp);

	std::getline(iss, tmp);
	this->setStatusMessage(tmp);
}

void	Response::updateStartLine(void) {
	this->setStartLine(this->httpVersion_ + " " + this->statusCode_ + " " + this->statusMessage_);
}
