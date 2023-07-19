/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:16:41 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 15:28:37 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cookie.hpp"
#include "Timer.hpp"
#include <cstdlib>

Cookie::Cookie(void) :	cookieId_(""),
						content(""),
						expireTime_(0) {

	this->generateCookieId();
	this->updateExpireTime();
}

Cookie::Cookie(Cookie const& other) :	cookieId_(other.cookieId_),
										content(other.content),
										expireTime_(other.expireTime_) {
	if (this != &other) {
		*this = other;
	}
}

Cookie &Cookie::operator=(Cookie const& other) {
	if (this != &other) {
		this->cookieId_ = other.cookieId_;
		this->content = other.content;
		this->expireTime_ = other.expireTime_;
	}
	return (*this);
}

Cookie::~Cookie(void) {

}

std::string const&	Cookie::getCookieId(void) const {
	return (this->cookieId_);
}

std::string const&	Cookie::getContent(void) const {
	return (this->content);
}

void	Cookie::setCookieId(std::string const& cookieId) {
	this->cookieId_ = cookieId;
}

void	Cookie::setContent(std::string const& content) {
	this->content = content;
	this->updateExpireTime();
}

void	Cookie::addContent(std::string const& content) {
	this->content += content;
	this->updateExpireTime();
}

void	Cookie::updateExpireTime(void) {
	this->expireTime_ = Timer::getCurrentTime() + COOKIE_EXPIRE_TIME;
}

bool	Cookie::isExpired(void) const {
	return (Timer::getCurrentTime() > this->expireTime_);
}

void	Cookie::generateCookieId(void) {
	std::string		chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string		cookieId;
	int				len = chars.length();

	for (int i = 0; i < 32; i++) {
		cookieId += chars[std::rand() % len];
	}
	this->setCookieId(cookieId);
}
