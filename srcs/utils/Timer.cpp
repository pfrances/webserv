/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 19:33:37 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 14:27:39 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Timer.hpp"
#include <cstdlib>
#include <iostream>

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

void Timer::startTimer() {
	std::srand(std::time(NULL));
	startTime_ = std::time(NULL);
}

double Timer::getElapsedTimeSince(std::time_t time) const {
	std::time_t currentTime = std::time(NULL);
	double elapsedTime = std::difftime(currentTime, time);
	return elapsedTime;
}

double Timer::getElapsedTimeSince(void) const {
	return getElapsedTimeSince(startTime_);
}

double Timer::getCurrentTime(void) {
	return std::time(NULL);
}

void Timer::printLogTime() const {
	std::time_t currentTime = std::time(NULL);
	std::string timeStr = std::asctime(std::localtime(&currentTime));
	timeStr.erase(timeStr.end() - 1);
	std::cout << "[" << timeStr << "] ";

}
