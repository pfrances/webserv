/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteReq.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:43:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 13:21:29 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteReq.hpp"
#include "Response.hpp"
#include "ServConf.hpp"

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

Response*	DeleteReq::execute(ServConf const& conf) const {
	(void)conf;
	return NULL;
}

bool	DeleteReq::isValidReq(void) const {
	return true;
}
