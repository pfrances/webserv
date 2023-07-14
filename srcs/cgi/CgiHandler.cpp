/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:44:51 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 19:37:56 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <stdexcept>
#include <cstdlib>
#include <signal.h>
#include <iostream>

CgiHandler::CgiHandler(void) : pid_(-1), cgiPath_(""), cgiExecutor_(""), pipe_(), envVec_(), argsVec_() {
	pipe_[0] = -1;
	pipe_[1] = -1;
}

CgiHandler::CgiHandler(std::string const& cgiPath, std::string const& cgiExecutor)
	: pid_(-1), cgiPath_(cgiPath), cgiExecutor_(cgiExecutor), pipe_(), envVec_(), argsVec_() {
	pipe_[0] = -1;
	pipe_[1] = -1;
	if (!cgiExecutor_.empty()) {
		this->argsVec_.push_back(cgiExecutor_);
	}
	this->argsVec_.push_back(cgiPath);
}

CgiHandler::CgiHandler(CgiHandler const& other)
	: pid_(other.pid_), cgiPath_(other.cgiPath_), cgiExecutor_(other.cgiExecutor_), envVec_(other.envVec_), argsVec_(other.argsVec_) {
	pipe_[0] = other.pipe_[0];
	pipe_[1] = other.pipe_[1];
}

CgiHandler &CgiHandler::operator=(CgiHandler const& other) {
	if (this != &other) {
		pid_ = other.pid_;
		cgiPath_ = other.cgiPath_;
		cgiExecutor_ = other.cgiExecutor_;
		pipe_[0] = other.pipe_[0];
		pipe_[1] = other.pipe_[1];
		envVec_ = other.envVec_;
		argsVec_ = other.argsVec_;
	}
	return *this;
}

CgiHandler::~CgiHandler(void) {
	if (pipe_[0] != -1) {
		close(pipe_[0]);
	}
	if (pipe_[1] != -1) {
		close(pipe_[1]);
	}
	if (pid_ != -1) {
		kill(pid_, SIGKILL);
		waitpid(pid_, NULL, 0);
	}
}

pid_t const& CgiHandler::getPid(void) const {
	return pid_;
}

int CgiHandler::getPipeReadFd(void) const {
	return pipe_[0];
}

int CgiHandler::getPipeWriteFd(void) const {
	return pipe_[1];
}

std::vector<std::string> const& CgiHandler::getEnv(void) const {
	return envVec_;
}

std::string const& CgiHandler::getCgiPath(void) const {
	return cgiPath_;
}

int CgiHandler::getClientFd(void) const {
	return clientFd_;
}

size_t CgiHandler::getStartTime(void) const {
	return startTime_;
}

void CgiHandler::setCgiPath(std::string const& path) {
	cgiPath_ = path;
}

void	CgiHandler::setClientFd(int fd) {
	this->clientFd_ = fd;
}

void	CgiHandler::setStartTime(size_t time) {
	this->startTime_ = time;
}

void CgiHandler::executeCgi(void) {
	if (pipe(pipe_) < 0) {
		throw std::runtime_error("pipe error");
	}

	pid_ = fork();
	if (pid_ == 0) {
		dup2(pipe_[0], STDIN_FILENO);
		dup2(pipe_[1], STDOUT_FILENO);
		close(pipe_[1]);
		close(pipe_[0]);
		char *const *args = StringVecToCharPtrArray(this->argsVec_);
		char *const *env = StringVecToCharPtrArray(this->envVec_);

		if (execve(args[0], args, env) < 0) {
			delete[] args;
			delete[] env;
			throw std::runtime_error("execve error");
		}
	} else if (pid_ > 0) {
		close(pipe_[1]);
		pipe_[1] = -1;
	} else {
		throw std::runtime_error("fork error");
	}
}

char *const*	CgiHandler::StringVecToCharPtrArray(std::vector<std::string> const& vec) const {
	char **ret = new char*[vec.size() + 1];
	for (size_t i = 0; i < vec.size(); ++i) {
		ret[i] = const_cast<char *>(vec.at(i).c_str());
	}
	ret[vec.size()] = NULL;
	return ret;
}
