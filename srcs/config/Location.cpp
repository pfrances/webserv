/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:23:49 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/18 15:37:04 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) {

}

Location::Location(const Location &other) :	path_(other.path_),
											root_(other.root_),
											index_(other.index_),
											tryFiles_(other.tryFiles_),
											errorPages_(other.errorPages_),
											redirect_(other.redirect_),
											allowedMethods_(other.allowedMethods_),
											cgiPath_(other.cgiPath_),
											cgiExtension_(other.cgiExtension_),
											clientMaxBodySize_(other.clientMaxBodySize_),
											directoryListing_(other.directoryListing_) {

}

Location &Location::operator=(const Location &other) {
	if (this != &other) {
		this->path_ = other.path_;
		this->root_ = other.root_;
		this->index_ = other.index_;
		this->tryFiles_ = other.tryFiles_;
		this->errorPages_ = other.errorPages_;
		this->redirect_ = other.redirect_;
		this->allowedMethods_ = other.allowedMethods_;
		this->cgiPath_ = other.cgiPath_;
		this->cgiExtension_ = other.cgiExtension_;
		this->clientMaxBodySize_ = other.clientMaxBodySize_;
		this->directoryListing_ = other.directoryListing_;
	}
	return (*this);
}

Location::~Location(void) {

}

void Location::parseLocationConf(std::string locationConfig) {

}
