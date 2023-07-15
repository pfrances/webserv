/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 12:30:35 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/15 17:19:45 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "File.hpp"
#include "ParseTools.hpp"
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <sstream>

File::File(void) :	Path(),
					fileName_(""),
					fileBaseName_(""),
					extension_(""),
					fileContent_("") {

}

File::File(std::string const& path) :	Path(path),
										fileName_(""),
										fileBaseName_(""),
										extension_(""),
										fileContent_("") {
	this->parsePath();
}

File::File(File const& other) :	Path(other),
								fileName_(other.fileName_),
								fileBaseName_(other.fileBaseName_),
								extension_(other.extension_),
								fileContent_(other.fileContent_) {

}

File &File::operator=(File const& other) {
	if (this != &other) {
		this->Path::operator=(other);
		this->fileName_ = other.fileName_;
		this->fileBaseName_ = other.fileBaseName_;
		this->extension_ = other.extension_;
		this->fileContent_ = other.fileContent_;
	}
	return (*this);
}

File::~File(void) {

}

std::string const&	File::getFileName(void) const {
	return (this->fileName_);
}

std::string const&	File::getFileBaseName(void) const {
	return (this->fileBaseName_);
}

std::string const&	File::getExtension(void) const {
	return (this->extension_);
}

std::string const&	File::getFileContent(void) {
	if (this->fileContent_.empty()) {
		this->read();
	}
	return (this->fileContent_);
}

std::map<std::string, std::string>	File::getFilesListing(void) const {

	DIR* dir = opendir(this->getFullPath().c_str());
	if (dir == NULL) {
		throw OpeningFailed("Cannot open directory");
	}

	std::map<std::string, std::string> FilesList;
	std::string fileName;
	std::string fileSize;
	dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		fileName = entry->d_name;
		if (entry->d_type == DT_DIR) {
			fileName += "/";
		}

		struct stat fileStat;
		std::string filePath = this->getFullPath() + "/" + fileName;
		if (stat(filePath.c_str(), &fileStat) == 0) {
			fileSize = ParseTools::intToString(fileStat.st_size);
		} else {
			fileSize = "????";
		}
		FilesList.insert(std::pair<std::string, std::string>(fileName, fileSize));
	}
	closedir(dir);
	return FilesList;
}

size_t	File::getFileSize(void) const {
	return (this->fileContent_.size());
}

void	File::setFileName(std::string const& fileName) {
	this->fileName_ = fileName;
}

void	File::setFileBaseName(std::string const& fileBaseName) {
	this->fileBaseName_ = fileBaseName;
}

void	File::setExtension(std::string const& extension) {
	this->extension_ = extension;
}

void	File::setFileContent(std::string const& content) {
	if (this->fileContent_ != content) {
		this->fileContent_ = content;
		this->write();
	}
}

bool	File::exists(void) const {
	std::ifstream	file(this->getFullPath().c_str());
	bool			exists = file.good();

	file.close();
	return (exists);
}

bool File::isRegularFile() const {
	struct stat fileStat;
	if (stat(getFullPath().c_str(), &fileStat) != 0) {
		return false;
	}
	return S_ISREG(fileStat.st_mode);
}

bool File::isDirectory() const {
	struct stat fileStat;
	if (stat(getFullPath().c_str(), &fileStat) != 0) {
		return false;
	}
	return S_ISDIR(fileStat.st_mode);
}

bool	File::isExecutable(void) const {
	return (this->exists() && access(this->getFullPath().c_str(), X_OK) == 0);
}

bool	File::isReadable(void) const {
	return (this->exists() && access(this->getFullPath().c_str(), R_OK) == 0);
}

bool	File::isWritable(void) const {
	return (this->exists() && access(this->getFullPath().c_str(), W_OK) == 0);
}

bool	File::isHidden(void) const {
	return (this->fileName_[0] == '.');
}

void	File::parsePath(void) {
	std::string const&	path = this->getFullPath();
	size_t				lastSlash = path.find_last_of('/');
	size_t				lastDot = path.find_last_of('.');
	size_t				lastSeparator = path.find_last_of("/.");

	if (lastSlash == std::string::npos) {
		this->fileName_ = path.substr(0, lastDot);
		this->extension_ = path.substr(lastDot + 1);
	} else if (lastDot == std::string::npos) {
		this->fileName_ = path.substr(lastSlash + 1);
	} else if (lastSlash > lastDot) {
		this->fileName_ = path.substr(lastSlash + 1);
	} else {
		this->fileName_ = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
		this->extension_ = path.substr(lastDot + 1);
	}
	if (lastSeparator == std::string::npos) {
		this->fileBaseName_ = this->fileName_;
	} else {
		this->fileBaseName_ = path.substr(lastSeparator + 1);
	}
}

void	File::write(void) {

	std::ofstream	file(this->getFullPath().c_str(), std::ios::binary);
	if (file.is_open()) {
		file << this->fileContent_;
		file.close();
	} else {
		throw OpeningFailed("Cannot open file");
	}
}

void	File::read(void) {
	std::ifstream	file(this->getFullPath().c_str());

	if (file.is_open()) {
		std::string	line;
		while (std::getline(file, line)) {
			this->fileContent_ += line + "\n";
		}
		file.close();
	} else {
		throw OpeningFailed("Cannot open file");
	}
}

void	File::append(std::string const& content) {
	std::ofstream	file(this->getFullPath().c_str(), std::ios_base::app);

	if (file.is_open()) {
		file.write(content.c_str(), content.size());
		file.close();
	} else {
		throw OpeningFailed("Cannot open file");
	}
	this->fileContent_ += content;
}

void File::setKeyValue(std::string const& key, std::string const& value) {
	if (!ParseTools::isValidToken(key, "=\"'")) {
		throw UnvalidTokenException("invalid key");
	}
	if (!ParseTools::isValidToken(value, "=\"'")) {
		throw UnvalidTokenException("invalid value");
	}

	if (!this->isReadable()) {
		this->setFileContent(key + "=" + value + "\n");
		return;
	}
	bool keyFound = false;
	std::istringstream iss(this->getFileContent());
	std::string buff;
	std::string newContent;
	newContent.reserve(this->getFileContent().length());
	while (std::getline(iss, buff)) {
		std::string lineKey = buff.substr(0, buff.find("="));
		std::string lineValue = buff.substr(buff.find("=") + 1);
		if (lineKey == key) {
			newContent += key + "=" + value + "\n";
			keyFound = true;
		} else {
			newContent += buff + "\n";
		}
	}
	if (!keyFound) {
		newContent += key + "=" + value + "\n";
	}
	this->setFileContent(newContent);
}

void File::unsetKey(std::string const& key) {
	if (!ParseTools::isValidToken(key, "=\"'")) {
		throw UnvalidTokenException("invalid key");
	}

	if (!this->isReadable()) {
		return;
	}

	std::istringstream iss(this->getFileContent());
	std::string buff;
	std::string newContent;
	newContent.reserve(this->getFileContent().length());
	while (std::getline(iss, buff)) {
		std::string lineKey = buff.substr(0, buff.find("="));
		std::string lineValue = buff.substr(buff.find("=") + 1);
		if (lineKey != key) {
			newContent += buff + "\n";
		}
	}
	this->setFileContent(newContent);
}

std::ostream &operator<<(std::ostream &o, File& file) {
	o << file.getFileContent();
	return (o);
}
