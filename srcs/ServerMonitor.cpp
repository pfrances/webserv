/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMonitor.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 15:37:47 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:20:13 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include "ConfParser.hpp"

ServerMonitor::ServerMonitor(std::string confFileName) {
	servConfs_ = ConfParser::parseMultiServConf(confFileName);

}

ServerMonitor::~ServerMonitor(void) {

}

ServerMonitor::ServerMonitor(const ServerMonitor &other) {

}

ServerMonitor &ServerMonitor::operator=(const ServerMonitor &other) {
	if (this != &other) {

	}
	return (*this);
}
