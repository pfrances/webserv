/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 12:27:17 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 16:54:10 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"

HttpMessage::HttpMessage(void) :	rawMessage(""),
									startLine_(""),
									headers_(),
									body_("") {

}

HttpMessage::HttpMessage(std::string const& rawMessage) : rawMessage_(rawMessage) {
	this->parseRawMessage();
}

HttpMessage::HttpMessage(HttpMessage const& other) :	rawMessage_(other.rawMessage_),
														startLine_(other.startLine_),
														headers_(other.headers_),
														body_(other.body_) {
	if (this != &other) {
		*this = other;
	}
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& other) {
	if (this != &other) {
		this->rawMessage_ = other.rawMessage_;
		this->startLine_ = other.startLine_;
		this->headers_ = other.headers_;
		this->body_ = other.body_;
	}
	return (*this);
}

HttpMessage::~HttpMessage(void) {

}

/****************************Getters****************************/
std::string const	HttpMessage::getStartLine(void) const {
	updateStartLine();
	return (this->startLine_);
}

std::map<std::string, std::string>const&	HttpMessage::getHeadersMap(void) const {
	return (this->headersMap_);
}

std::string const&	HttpMessage::getSingleHeader(std::string const& key) const {
	return (this->headers_.at(key));
}

std::string const&	HttpMessage::getBody(void) const {
	return (this->body_);
}

std::string const&	HttpMessage::getRawMessage(void) const {
	updateRawMessage();
	return (this->rawMessage_);
}

/****************************Setters****************************/
void	HttpMessage::setStartLine(std::string const& firstLine) {
	this->startLine_ = firstLine;
}

void	HttpMessage::setSingleHeader(std::string const& key, std::string const& value) {
	this->headers_[key] = value;
}

void	HttpMessage::setHeadersMap(std::map<std::string, std::string> const& headers) {
	this->headers_ = headers;
}

void	HttpMessage::setBody(std::string const& body) {
	this->body_ = body;
}

void	HttpMessage::setRawMessage(std::string const& rawMessage) {
	this->rawMessage_ = rawMessage;
	parseRawMessage();
}

/****************************Parsers****************************/
void	HttpMessage::parseRawMessage(std::string const& rawMessage) {
	std::string		parseLine = rawMessage;

	std::string		startLine = parseLine.substr(0, parseLine.find(CRLF));
	parseLine = parseLine.substr(parseLine.find(CRLF) + 2);

	std::string		headersStr = parseLine.substr(0, parseLine.find(CRLF + CRLF));
	std::string		body = parseLine.substr(parseLine.find(CRLF + CRLF) + 4);

	this->startLine_ = parseStartLine(startLine);
	this->headers_ = parseHeaders(headersStr);
	this->body_ = body
	this->rawMessage_ = rawMessage;
}

std::pair<std::string, std::string>	HttpMessage::parseSingleHeader(std::string const& header) {
	std::string		key;
	std::string		value;

	key = header.substr(0, header.find(": "));
	value = header.substr(header.find(": ") + 2);

	return (std::make_pair(key, value));
}

void	HttpMessage::parseHeaders(std::string const& headersStr) {
	std::string		parseLine = headersStr;
	std::string		header;

	while (!parseLine.empty()) {
		header = parseLine.substr(0, parseLine.find(CRLF));
		parseLine = parseLine.substr(parseLine.find(CRLF) + 2);
		this->headers_.insert(parseSingleHeader(header));
	}
}

/****************************Others****************************/
bool	HttpMessage::hasStartLine(void) const {
	return (!this->startLine_.empty());
}

bool	HttpMessage::hasHeaders(void) const {
	return (!this->headers_.empty());
}

bool	HttpMessage::hasHeaderEntry(std::string const& key) const {
	return (this->headers_.find(key) != this->headers_.end());
}

bool	HttpMessage::hasBody(void) const {
	return (!this->body_.empty());
}

bool	HttpMessage::hasRawMessage(void) const {
	return (!this->rawMessage_.empty());
}

void	HttpMessage::updateRawMessage(void) {
	std::string::iterator it = this->headers_.begin();
	std::string::iterator ite = this->headers_.end();

	std::string rawMessage = this->startLine_ + CLRF;
	for (; it != ite; ++it) {
		rawMessage += it->first + ": " + it->second + CLRF;
	}
	rawMessage += CLRF + this->body_;

	if (this->rawMessage_ != rawMessage) {
		this->rawMessage_ = rawMessage;
		parseRawMessage();
	}
}
