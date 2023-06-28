/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:45 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/28 19:03:38 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <sstream>

Request::Request(void) :	HttpMessage(),
							method_(""),
							uri_(""),
							query_(),
							httpVersion_(""),
							clientSocket_(-1) {

}

Request::Request(std::string const& rawRequest, int clientSocket) :	HttpMessage(rawRequest),
																	method_(""),
																	uri_(""),
																	query_(),
																	httpVersion_(""),
																	clientSocket_(clientSocket) {
	parseStartLine();
}

Request::Request(Request const& other) :	HttpMessage(other),
											method_(other.method_),
											uri_(other.uri_),
											query_(other.query_),
											httpVersion_(other.httpVersion_),
											clientSocket_(other.clientSocket_) {

}

Request &Request::operator=(Request const& other) {
	if (this != &other) {
		HttpMessage::operator=(other);
		this->method_ = other.method_;
		this->uri_ = other.uri_;
		this->query_ = other.query_;
		this->httpVersion_ = other.httpVersion_;
		this->clientSocket_ = other.clientSocket_;
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

void	Request::setQuery(std::string const& queryStr) {
	std::string::const_iterator it = queryStr.begin();
	std::string::const_iterator ite = queryStr.end();

	std::string key;
	std::string token;
	for (; it != ite; it++) {
		if (*it == '=') {
			key = token;
			token = "";
		}
		else if (*it == '&') {
			this->query_[key] = token;
			key = "";
			token = "";
		}
		else {
			token += *it;
		}
	}
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
	if (tmp.find('?') != std::string::npos) {
		this->setUri(tmp.substr(0, tmp.find('?')));
		this->setQuery(tmp.substr(tmp.find('?') + 1));
	}
	else {
		this->setUri(tmp);
	}

	std::getline(iss, tmp);
	this->setHttpVersion(tmp);
}

void	Request::updateStartLine(void) {
	this->setStartLine(this->method_ + " " + this->uri_ + " " + this->httpVersion_);
}
