/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:42:22 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:25:54 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostReq.hpp"
#include "Response.hpp"
#include "Server.hpp"

PostReq::PostReq(void) :	Request() {

}

PostReq::PostReq(std::string const& rawRequest) :	Request(rawRequest) {

}

PostReq::PostReq(PostReq const& other) :	Request(other) {

}

PostReq &PostReq::operator=(PostReq const& other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

PostReq::~PostReq(void) {

}

Response*	PostReq::execute(Server const& conf) const {
	(void)conf;
	return NULL;
}


bool	PostReq::isValidReq(void) const {
	return true;
}
