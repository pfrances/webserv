/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetReq.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:42:59 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 20:10:16 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetReq.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <iostream>

GetReq::GetReq(void) :	Request() {

}

GetReq::GetReq(std::string const& rawRequest) :	Request(rawRequest) {

}

GetReq::GetReq(GetReq const& other) :	Request(other) {

}

GetReq &GetReq::operator=(GetReq const& other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

GetReq::~GetReq(void) {

}

Response*	GetReq::execute(ServerMonitor& serverMonitor) const {
	(void)serverMonitor;
	Response *res = new Response();
	res->setStartLine("HTTP/1.1 200 OK");
	res->setSingleHeader("Content-Type", "text/plain");
	res->setSingleHeader("Content-Length", "13");
	res->setBody("Hello World!!");
	return res;
}

bool	GetReq::isValidReq(void) const {
	return true;
}
