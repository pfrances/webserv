/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:45 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:49:37 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetReq.hpp"
#include "PostReq.hpp"
#include "DeleteReq.hpp"
#include "ServConf.hpp"
#include <iostream>

Request::Request(void) :	HttpMessage(),
							method_(""),
							uri_(""),
							httpVersion_("") {

}

Request::Request(std::string rawRequest) :	HttpMessage(rawRequest),
											method_(""),
											uri_(""),
											httpVersion_("") {
	parseStartLine();
}

Request::Request(const Request &other) :	HttpMessage(other),
											method_(other.method_),
											uri_(other.uri_),
											httpVersion_(other.httpVersion_) {

}

Request &Request::operator=(const Request &other) {
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
}

void	Request::setUri(std::string const& uri) {
	this->uri_ = uri;
}

void	Request::setHttpVersion(std::string const& httpVersion) {
	this->httpVersion_ = httpVersion;
}

void	Request::parseStartLine(void) {
	std::string	startLine = getStartLine();

	this->method_ = startLine.substr(0, startLine.find(' '));
	startLine = startLine.substr(startLine.find(' ') + 1);

	this->uri_ = startLine.substr(0, startLine.find(' '));
	startLine = startLine.substr(startLine.find(' ') + 1);

	this->httpVersion_ = startLine.substr(0, startLine.find('\r'));
}

void	Request::updateStartLine(void) {
	setStartLine(this->method_ + " " + this->uri_ + " " + this->httpVersion_);
}

Response const&	Request::generateResponse(ServConf const& conf) {
	process(conf);
	return res_;
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
