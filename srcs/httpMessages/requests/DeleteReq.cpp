/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteReq.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:43:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:43:38 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteReq.hpp"

DeleteReq::DeleteReq(void) :	Request() {

}

DeleteReq::DeleteReq(std::string rawRequest) :	Request(rawRequest) {

}

DeleteReq::DeleteReq(const DeleteReq &other) :	Request(other) {

}

DeleteReq &DeleteReq::operator=(const DeleteReq &other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

DeleteReq::~DeleteReq(void) {

}
