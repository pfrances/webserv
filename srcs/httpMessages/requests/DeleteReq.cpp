/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteReq.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:43:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:25:40 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteReq.hpp"
#include "Response.hpp"
#include "Server.hpp"

DeleteReq::DeleteReq(void) :	Request() {

}

DeleteReq::DeleteReq(std::string const& rawRequest) :	Request(rawRequest) {

}

DeleteReq::DeleteReq(DeleteReq const& other) :	Request(other) {

}

DeleteReq &DeleteReq::operator=(DeleteReq const& other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

DeleteReq::~DeleteReq(void) {

}

Response*	DeleteReq::execute(ServerMonitor& serverMonitor) const {
	(void)serverMonitor;
	return NULL;
}

bool	DeleteReq::isValidReq(void) const {
	return true;
}
