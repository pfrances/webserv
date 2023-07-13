/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 12:27:17 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 18:06:50 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpMessage.hpp"
#include "ParseTools.hpp"
#include "MimeTypes.hpp"
#include <sstream>
#include <iostream>

HttpMessage::HttpMessage(void) :	rawMessage_(""),
									startLine_(""),
									headersMap_(),
									body_(""),
									chunksFetched_(true),
									totalSize_(0),
									boundary_(""),
									hostName_("") {

}

HttpMessage::HttpMessage(std::string const& rawMessage) :	rawMessage_(rawMessage),
															startLine_(""),
															headersMap_(),
															body_(""),
															chunksFetched_(true),
															totalSize_(0),
															boundary_(""),
															hostName_("") {
	parseRawMessage();
}

HttpMessage::HttpMessage(HttpMessage const& other) :	rawMessage_(other.rawMessage_),
														startLine_(other.startLine_),
														headersMap_(other.headersMap_),
														body_(other.body_),
														chunksFetched_(other.chunksFetched_),
														totalSize_(other.totalSize_),
														boundary_(other.boundary_),
														hostName_(other.hostName_) {
	if (this != &other) {
		*this = other;
	}
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& other) {
	if (this != &other) {
		this->rawMessage_ = other.rawMessage_;
		this->startLine_ = other.startLine_;
		this->headersMap_ = other.headersMap_;
		this->body_ = other.body_;
		this->chunksFetched_ = other.chunksFetched_;
		this->totalSize_ = other.totalSize_;
		this->boundary_ = other.boundary_;
		this->hostName_ = other.hostName_;
	}
	return (*this);
}

HttpMessage::~HttpMessage(void) {

}

/****************************Getters****************************/
std::string const&	HttpMessage::getStartLine(void) const {
	return (this->startLine_);
}

std::string const&	HttpMessage::getHeadersStr(void) const {
	return (this->headersStr_);
}

std::map<std::string, std::string>const&	HttpMessage::getHeadersMap(void) const {
	return (this->headersMap_);
}

std::string const&	HttpMessage::getSingleHeader(std::string const& key) const {
	return (this->headersMap_.at(key));
}

std::string const&	HttpMessage::getBody(void) const {
	return (this->body_);
}

std::string const&	HttpMessage::getRawMessage(void) const {
	return (this->rawMessage_);
}

std::string const&		HttpMessage::getHostName(void) const{
	return this->hostName_;
}

bool	HttpMessage::isFetched(void) const {
	return this->chunksFetched_;
}

/****************************Setters****************************/
void	HttpMessage::setStartLine(std::string const& startLine) {
	this->startLine_ = startLine;
	this->updateRawMessage();
}

void	HttpMessage::setHeadersStr(std::string const& headersStr) {
	this->headersStr_ = headersStr;
	this->parseHeadersMap();
	this->updateRawMessage();
}

void	HttpMessage::setSingleHeader(std::string const& key, std::string const& value) {
	this->headersMap_[key] = value;
	this->updateHeadersStr();
	this->updateRawMessage();
	if (key == "Host") {
		this->hostName_ = value;
		this->hostName_ = value.substr(0, value.find(":"));
	}
}

void	HttpMessage::setHeadersMap(std::map<std::string, std::string> const& headers) {
	this->headersMap_ = headers;
	this->updateHeadersStr();
	this->updateRawMessage();

	std::string const& host = this->getSingleHeader("Host");
	this->hostName_ = host.substr(0, host.find(":"));
}

void	HttpMessage::setBody(std::string const& body) {
	this->body_ = body;
	this->setSingleHeader("Content-Length", ParseTools::intToString(body.length()));
	this->updateRawMessage();
}

void	HttpMessage::appendToBody(std::string const& chunk) {
	this->body_ += chunk;
	this->setSingleHeader("Content-Length", ParseTools::intToString(this->body_.length()));
	this->updateRawMessage();
	if (this->body_.length() == this->totalSize_) {
		this->chunksFetched_ = true;
	}
}

void	HttpMessage::setRawMessage(std::string const& rawMessage) {
	this->rawMessage_ = rawMessage;
	parseRawMessage();
}

void	HttpMessage::setMimeByExtension(std::string const& ext) {
	this->setSingleHeader("Content-Type", MimeTypes::getMimeType(ext));
}

/****************************Parsers****************************/
void	HttpMessage::parseRawMessage() {

	std::istringstream iss(this->rawMessage_);
	std::string startLine;
	std::getline(iss, startLine);
	this->startLine_ = startLine.substr(0, startLine.find("\r"));
	// the startLine_ has to be parsed on the child class

	std::string header;
	while (std::getline(iss, header) && header != "\r") {
		this->headersStr_ += header + CRLF;
	}
	this->parseHeadersMap();

	std::string body;
	std::getline(iss, body, '\0');
	if (!body.empty())
		this->parseBody(body);
}

std::pair<std::string, std::string>	HttpMessage::parseSingleHeader(std::string const& header) {
	std::string		key;
	std::string		value;

	key = header.substr(0, header.find(": "));
	value = header.substr(header.find(": ") + 2);

	return (std::make_pair(key, value));
}

void	HttpMessage::parseHeadersMap(void) {

	std::istringstream iss(this->headersStr_);

	std::string header;
	while (std::getline(iss, header)) {
		std::pair<std::string, std::string> headerPair = parseSingleHeader(header);
		this->setSingleHeader(headerPair.first, headerPair.second);
	}
}

void	HttpMessage::parseBody(std::string const& body) {

	if (this->headersMap_.find("Content-Length") != this->headersMap_.end()) {
		this->totalSize_ = ParseTools::stringToInt(this->getSingleHeader("Content-Length"));
	}
	std::string const& contentType = this->getSingleHeader("Content-Type");

	if (contentType.find("multipart/form-data") != std::string::npos) {
		this->boundary_ = contentType.substr(contentType.find("bundary=") + 8);
		this->chunksFetched_ = false;
		if (body.length() > 0) {
			this->setBody(body);
		}
	} else {
		this->setBody(body);
	}
}

/****************************Others****************************/
bool	HttpMessage::hasStartLine(void) const {
	return (!this->startLine_.empty());
}

bool	HttpMessage::hasHeaders(void) const {
	return (!this->headersMap_.empty());
}

bool	HttpMessage::hasHeaderEntry(std::string const& key) const {
	return (this->headersMap_.find(key) != this->headersMap_.end());
}

bool	HttpMessage::hasBody(void) const {
	return (!this->body_.empty());
}

bool	HttpMessage::hasRawMessage(void) const {
	return (!this->rawMessage_.empty());
}

/*********************************Updaters****************************************/

void	HttpMessage::updateHeadersStr(void) {
	std::map<std::string, std::string>::iterator it = this->headersMap_.begin();
	std::map<std::string, std::string>::iterator ite = this->headersMap_.end();

	this->headersStr_ = std::string("");
	for (; it != ite; it++) {
		this->headersStr_ += it->first + ": " + it->second + CRLF;
	}
}

void	HttpMessage::updateRawMessage(void) {
	this->rawMessage_ = this->startLine_ + CRLF + this->headersStr_ + CRLF + this->body_;
}
