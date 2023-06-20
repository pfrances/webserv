/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:42:22 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:42:54 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostReq.hpp"

PostReq::PostReq(void) :	Request() {

}

PostReq::PostReq(std::string rawRequest) :	Request(rawRequest) {

}

PostReq::PostReq(const PostReq &other) :	Request(other) {

}

PostReq &PostReq::operator=(const PostReq &other) {
	if (this != &other) {
		Request::operator=(other);
	}
	return (*this);
}

PostReq::~PostReq(void) {

}
