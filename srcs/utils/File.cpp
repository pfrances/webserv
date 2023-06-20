/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 12:30:35 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/20 12:37:43 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"

File::File(void) :	Path(),
					name_(""),
					extension_(""),
					content_("") {

}

File::File(std::string const& path) :	Path(path) {
	this->parsePath();
}

File::File(File const& other) :	Path(other),
								name_(other.name_),
								extension_(other.extension_),
								content_(other.content_) {
	if (this != &other) {
		*this = other;
	}
}

File &File::operator=(File const& other) {
	if (this != &other) {
		this->Path::operator=(other);
		this->name_ = other.name_;
		this->extension_ = other.extension_;
		this->content_ = other.content_;
	}
	return (*this);
}

File::~File(void) {

}

std::string const&	File::getName(void) const {
	return (this->name_);
}

std::string const&	File::getExtension(void) const {
	return (this->extension_);
}

std::string const&	File::getContent(void) const {
	return (this->content_);
}

void	File::parsePath(void) {
	std::string const&	path = this->getFullPath();
	size_t				lastSlash = path.find_last_of('/');
	size_t				lastDot = path.find_last_of('.');
	size_t				lastSeparator = path.find_last_of("/.");

	if (lastSlash == std::string::npos) {
		this->name_ = path.substr(0, lastDot);
		this->extension_ = path.substr(lastDot + 1);
	} else if (lastDot == std::string::npos) {
		this->name_ = path.substr(lastSlash + 1);
	} else if (lastSlash > lastDot) {
		this->name_ = path.substr(lastSlash + 1);
	} else {
		this->name_ = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
		this->extension_ = path.substr(lastDot + 1);
	}
}

void	File::read(void) {
	std::ifstream	file(this->getFullPath().c_str());

	if (file.is_open()) {
		std::stringstream	buffer;

		buffer << file.rdbuf();
		this->content_ = buffer.str();
		file.close();
	} else {
		throw File::FileException("Cannot open file");
	}
}

void	File::write(void) {
	std::ofstream	file(this->getFullPath().c_str());

	if (file.is_open()) {
		file << this->content_;
		file.close();
	} else {
		throw File::FileException("Cannot open file");
	}
}

void	File::write(std::string const& content) {
	this->content_ = content;
	this->write();
}

std::ostream &operator<<(std::ostream &o, File const& file) {
	o << file.getContent();
	return (o);
}
