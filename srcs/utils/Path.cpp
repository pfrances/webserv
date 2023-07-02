/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 11:32:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/30 11:22:40 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Path.hpp"

Path::Path(void) :	absolutePath_(""),
					relativePath_(""),
					absolutePathList_(),
					relativePathList_() {

}

Path::Path(std::string const& path) {
	this->setPath(path);
}

Path::Path(Path const& other) : absolutePath_(other.absolutePath_),
								relativePath_(other.relativePath_),
								absolutePathList_(other.absolutePathList_),
								relativePathList_(other.relativePathList_) {
	if (this != &other) {
		*this = other;
	}
}

Path &Path::operator=(const Path &other) {
	if (this != &other) {
		this->absolutePath_ = other.absolutePath_;
		this->relativePath_ = other.relativePath_;
		this->absolutePathList_ = other.absolutePathList_;
		this->relativePathList_ = other.relativePathList_;
	}
	return (*this);
}

Path::~Path(void) {

}

std::string const&	Path::getFullPath(void) const {
	return (this->absolutePath_);
}

std::string const&	Path::getRelativePath(void) const {
	return (this->relativePath_);
}

std::list<std::string> const&	Path::getFullPathList(void) const {
	return (this->absolutePathList_);
}

std::list<std::string> const&	Path::getRelativePathList(void) const {
	return (this->relativePathList_);
}

std::string	Path::getPathFrom(std::string const& path) const {
	std::string	result;
	std::list<std::string>::const_iterator	it;
	std::list<std::string>::const_iterator	ite;

	it = this->absolutePathList_.begin();
	ite = this->absolutePathList_.end();
	while (it != ite) {
		if (*it == path) {
			break ;
		}
		it++;
	}
	if (it != ite) {
		while (it != ite) {
			result += "../";
			it++;
		}
	}
	else {
		result = this->absolutePath_;
	}
	return (result);
}

std::string	Path::getPathTo(std::string const& path) const {
	std::string	result;
	std::list<std::string>::const_iterator	it;
	std::list<std::string>::const_iterator	ite;

	it = this->absolutePathList_.begin();
	ite = this->absolutePathList_.end();
	while (it != ite) {
		if (*it == path) {
			break ;
		}
		it++;
	}
	if (it != ite) {
		while (it != ite) {
			result += "../";
			it++;
		}
	}
	else {
		result = this->absolutePath_;
	}
	return (result);
}

void	Path::setPath(std::string const& pathToSet) {
	std::string path = pathToSet;
	while (path.find("//") != std::string::npos) {
		path.replace(path.find("//"), 2, "/");
	}
	std::string	absolutePath;
	std::string	relativePath;
	std::list<std::string>	absolutePathList;
	std::list<std::string>	relativePathList;
	std::string::size_type	pos;
	std::string::size_type	prevPos;

	absolutePath = path;
	relativePath = path;
	pos = path.find("/");
	prevPos = 0;
	while (pos != std::string::npos) {
		absolutePathList.push_back(absolutePath.substr(prevPos, pos - prevPos));
		relativePathList.push_back(relativePath.substr(prevPos, pos - prevPos));
		prevPos = pos + 1;
		pos = path.find("/", prevPos);
	}
	absolutePathList.push_back(absolutePath.substr(prevPos, pos - prevPos));
	relativePathList.push_back(relativePath.substr(prevPos, pos - prevPos));
	this->absolutePath_ = absolutePath;
	this->relativePath_ = relativePath;
	this->absolutePathList_ = absolutePathList;
	this->relativePathList_ = relativePathList;
}
