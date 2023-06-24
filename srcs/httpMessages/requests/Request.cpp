/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:45 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:25:30 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetReq.hpp"
#include "PostReq.hpp"
#include "DeleteReq.hpp"
#include "Server.hpp"
#include <sstream>

Request::Request(void) :	HttpMessage(),
							method_(""),
							uri_(""),
							httpVersion_("") {

}

Request::Request(std::string const& rawRequest) :	HttpMessage(rawRequest),
													method_(""),
													uri_(""),
													httpVersion_("") {
	parseStartLine();
}

Request::Request(Request const& other) :	HttpMessage(other),
											method_(other.method_),
											uri_(other.uri_),
											httpVersion_(other.httpVersion_) {

}

Request &Request::operator=(Request const& other) {
	if (this != &other) {
		HttpMessage::operator=(other);
		this->method_ = other.method_;
		this->uri_ = other.uri_;
		this->httpVersion_ = other.httpVersion_;
	}
	return (*this);
}

Request::~Request(void) {

}

std::string const&	Request::getMethod(void) const {
	return (this->method_);
}

std::string const&	Request::getUri(void) const {
	return (this->uri_);
}

std::string const&	Request::getHttpVersion(void) const {
	return (this->httpVersion_);
}

void	Request::setMethod(std::string const& method) {
	this->method_ = method;
	this->updateStartLine();
	this->updateRawMessage();
}

void	Request::setUri(std::string const& uri) {
	this->uri_ = uri;
	this->updateStartLine();
	this->updateRawMessage();
}

void	Request::setHttpVersion(std::string const& httpVersion) {
	this->httpVersion_ = httpVersion;
	this->updateStartLine();
	this->updateRawMessage();
}

void	Request::parseStartLine(void) {

	std::istringstream iss(this->getStartLine());
	std::string tmp;

	std::getline(iss, tmp, ' ');
	this->setMethod(tmp);

	std::getline(iss, tmp, ' ');
	this->setUri(tmp);

	std::getline(iss, tmp);
	this->setHttpVersion(tmp);
}

void	Request::updateStartLine(void) {
	this->setStartLine(this->method_ + " " + this->uri_ + " " + this->httpVersion_);
}

Request*	createRequestFromRequestString(std::string const& rawRequest) {
	std::string	method = rawRequest.substr(0, rawRequest.find(' '));

	if (method == "GET") {
		return new GetReq(rawRequest);
	} else if (method == "POST") {
		return new PostReq(rawRequest);
	} else if (method == "DELETE") {
		return new DeleteReq(rawRequest);
	}
	return NULL;
}
