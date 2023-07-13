/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:22:45 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 12:40:38 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "ParseTools.hpp"
#include <iostream>
#include <sstream>

Request::Request(void) :	HttpMessage(),
							method_(""),
							uri_(""),
							query_(),
							uriWithQuery_(""),
							httpVersion_("") {

}

Request::Request(std::string const& rawRequest) :	HttpMessage(rawRequest),
													method_(""),
													uri_(""),
													query_(),
													uriWithQuery_(""),
													httpVersion_("") {
	parseStartLine();
}

Request::Request(Request const& other) :	HttpMessage(other),
											method_(other.method_),
											uri_(other.uri_),
											query_(other.query_),
											uriWithQuery_(other.uriWithQuery_),
											httpVersion_(other.httpVersion_) {

}

Request &Request::operator=(Request const& other) {
	if (this != &other) {
		HttpMessage::operator=(other);
		this->method_ = other.method_;
		this->uri_ = other.uri_;
		this->query_ = other.query_;
		this->uriWithQuery_ = other.uriWithQuery_;
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

std::string const& Request::getUriWithQuery(void) const {
	return (this->uriWithQuery_);
}

std::map<std::string, std::string> const&	Request::getQuery(void) const {
	return (this->query_);
}

std::string const&	Request::getHttpVersion(void) const {
	return (this->httpVersion_);
}

bool	Request::isMethodValid(void) const {
	return (this->method_ == "GET" || this->method_ == "POST" || this->method_ == "DELETE");
}

bool	Request::isUriValid(void) const {
	return (this->uri_.size() > 0);
}

bool	Request::isHttpVersionValid(void) const {
	return (this->httpVersion_ == "HTTP/1.1");
}

bool	Request::isRequestValid(void) const {
	return (this->isMethodValid() && this->isUriValid() && this->isHttpVersionValid());
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
	this->query_ = ParseTools::parseQuery(queryStr);
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
	this->uriWithQuery_ = tmp;
	size_t queryStartPos = tmp.find("?");
	if (queryStartPos != std::string::npos) {
		this->setUri(tmp.substr(0, queryStartPos));
		this->setQuery(tmp.substr(queryStartPos + 1));
	} else {
		this->setUri(tmp);
	}

	std::getline(iss, tmp);
	this->setHttpVersion(tmp);
}

void	Request::updateStartLine(void) {
	this->setStartLine(this->method_ + " " + this->uriWithQuery_ + " " + this->httpVersion_);
}

std::vector<Request*> parseMultipleRequest(std::string const& allMsgs) {
	std::vector<Request*> reqVec;

	std::istringstream	iss(allMsgs);
	std::string			reqStr;
	std::string			tmp;
	int					contentLength = 0;

	while(std::getline(iss, tmp)) {
		reqStr += tmp + "\n";
		if (tmp.find("Content-Length:") != std::string::npos) {
			contentLength = ParseTools::stringToInt(tmp.substr(15));
		} else if (tmp == "\r") {
			if (contentLength > 0) {
				std::string body;
				std::getline(iss, body, '\0');
				reqStr += body;
				contentLength = 0;
			}
			reqVec.push_back(new Request(reqStr));
			reqStr = "";
		}
	}
	return (reqVec);
}
