/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 19:33:37 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/17 11:29:56 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Timer.hpp"

Timer::Timer(void) : startTime_() {

}

Timer::Timer(Timer const& other) : startTime_(other.startTime_) {

}

Timer &Timer::operator=(Timer const& other) {
	if (this != &other) {
		this->startTime_ = other.startTime_;
	}
	return (*this);
}

Timer::~Timer(void) {

}

void	Timer::startTimer(void) {
	gettimeofday(&this->startTime_, NULL);
}

size_t	Timer::getCurrentTime(void) const {
	struct timeval	currentTime;
	size_t			elapsedTime;

	gettimeofday(&currentTime, NULL);
	elapsedTime = (currentTime.tv_sec - this->startTime_.tv_sec) * 1000;
	elapsedTime += (currentTime.tv_usec - this->startTime_.tv_usec) / 1000;
	return (elapsedTime);
}

size_t	Timer::getElapsedTimeSince(size_t time) const {
	struct timeval	currentTime;
	size_t			elapsedTime;

	gettimeofday(&currentTime, NULL);
	elapsedTime = (currentTime.tv_sec) * 1000 + (currentTime.tv_usec) / 1000 - time;
	return (elapsedTime);
}
