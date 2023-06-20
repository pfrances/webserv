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
#include <stdexcept>

ServConf::ServConf(void) :	serverName_("webserver"),
							host_("127.0.0.1"),
							port_(8080) {
	sockaddr_.sin_family = AF_INET;
	sockaddr_.sin_port = htons(port_);
	sockaddr_.sin_addr.s_addr = htonl(stringIpToInt(host_)); 

	pollfd_.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (pollfd_.fd < 0)
		throw std::runtime_error("socker error");
	pollfd_.events = POLLIN;
	pollfd_.revents = 0;
	if (bind(pollfd_.fd, (struct sockaddr *)&sockaddr_, sizeof(sockaddr_)) < 0)
		throw std::runtime_error("bind error");
}
ServConf::ServConf(std::string confFile) {
	(void)confFile;
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

pollfd const& ServConf::getPollFd(void) const {
	return (this->pollfd_);
}

void	ServConf::startListen(void) const {
	if (listen(pollfd_.fd, 10) < 0)
		throw std::runtime_error("listen error");
}

int	stringIpToInt(const std::string &ip) {
    std::string::const_iterator it = ip.begin();
    std::string::const_iterator ite = ip.end();
    int result = 0;
    int bit = 0;
    for (; it != ite; it++) {
        if (*it == '.') {
            result = (result << 8) | bit;
            bit = 0;
        } else {
            bit = bit * 10 + (*it - '0');
        }
    }
    result = (result << 8) | bit;
    return result;
}
