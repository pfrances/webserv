/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServConf.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:13:44 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/18 15:23:34 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServConf.hpp"

ServConf::ServConf(std::string confFile) {

}

ServConf::~ServConf(void) {

}

ServConf::ServConf(const ServConf &other) :	serverName_(other.serverName_),
											host_(other.host_),
											port_(other.port_),
											sockaddr_(other.sockaddr_),
											defaultLocation_(other.defaultLocation_),
											locations_(other.locations_) {

}

ServConf &ServConf::operator=(const ServConf &other) {
	if (this != &other) {
		this->serverName_ = other.serverName_;
		this->host_ = other.host_;
		this->port_ = other.port_;
		this->sockaddr_ = other.sockaddr_;
		this->defaultLocation_ = other.defaultLocation_;
		this->locations_ = other.locations_;
	}
	return (*this);
}

std::string const& ServConf::getServerName(void) const {
	return (this->serverName_);
}

std::string const& ServConf::getHost(void) const {
	return (this->host_);
}

int ServConf::getPort(void) const {
	return (this->port_);
}

struct sockaddr_in const& ServConf::getSockaddr(void) const {
	return (this->sockaddr_);
}

std::vector<Location> const& ServConf::getLocations(void) const {
	return (this->locations_);
}
