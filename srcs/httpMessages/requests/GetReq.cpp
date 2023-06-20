/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetReq.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:42:59 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:43:16 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetReq.hpp"
#include "ServConf.hpp"

GetReq::GetReq(void) :	Request() {

}

GetReq::GetReq(std::string rawRequest) :	Request(rawRequest) {

}

GetReq::GetReq(const GetReq &other) :	Request(other) {

}

GetReq &GetReq::operator=(const GetReq &other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

GetReq::~GetReq(void) {

}

void	GetReq::process(ServConf conf) {
	(void)conf;
	this->res_.setStartLine("HTTP/1.1 200 OK");
	this->res_.setSingleHeader("Content-Type", "text/plain");
	this->res_.setSingleHeader("Content-Length", "1024");
	this->res_.setBody("Hello World!!");
}

bool	GetReq::isValidReq(void) const {
	return true;
}
