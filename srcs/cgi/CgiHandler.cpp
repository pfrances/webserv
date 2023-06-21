/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:44:51 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 14:44:41 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler(void) {

}

CgiHandler::CgiHandler(CgiHandler const& other) {
	*this = other;
}

CgiHandler &CgiHandler::operator=(CgiHandler const& other) {
	if (this != &other) {

	}
	return (*this);
}

CgiHandler::~CgiHandler(void) {

}
