/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 14:24:50 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/29 15:49:51 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MimeTypes.hpp"

bool MimeTypes::isInit_ = false;
std::map<std::string, std::string> MimeTypes::mimeTypes_;

std::string const& MimeTypes::getMimeType(std::string const& extension) {
	if (!MimeTypes::isInit_) {
		MimeTypes::initMimeTypes();
	}

	if (MimeTypes::mimeTypes_.find(extension) != MimeTypes::mimeTypes_.end()) {
		return (MimeTypes::mimeTypes_[extension]);
	}

	return (MimeTypes::mimeTypes_["txt"]);
}

void MimeTypes::initMimeTypes(void) {
	mimeTypes_["txt"] = "text/plain";
	mimeTypes_["html"] = "text/html";
	mimeTypes_["htm"] = "text/html";
	mimeTypes_["css"] = "text/css";
	mimeTypes_["js"] = "text/javascript";
	mimeTypes_["json"] = "application/json";
	mimeTypes_["xml"] = "application/xml";
	mimeTypes_["pdf"] = "application/pdf";
	mimeTypes_["zip"] = "application/zip";
	mimeTypes_["tar"] = "application/x-tar";
	mimeTypes_["gif"] = "image/gif";
	mimeTypes_["png"] = "image/png";
	mimeTypes_["jpg"] = "image/jpeg";
	mimeTypes_["jpeg"] = "image/jpeg";
	mimeTypes_["jpe"] = "image/jpeg";
	mimeTypes_["ico"] = "image/x-icon";
	mimeTypes_["bmp"] = "image/bmp";
	mimeTypes_["mp3"] = "audio/mpeg";
	mimeTypes_["wav"] = "audio/wav";
	mimeTypes_["mp4"] = "video/mp4";
	mimeTypes_["avi"] = "video/x-msvideo";
	mimeTypes_["mpeg"] = "video/mpeg";
	mimeTypes_["ttf"] = "font/ttf";
	mimeTypes_["otf"] = "font/otf";
	mimeTypes_["csv"] = "text/csv";
	MimeTypes::isInit_ = true;
}
