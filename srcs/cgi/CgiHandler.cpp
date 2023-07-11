/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:44:51 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/10 09:45:48 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <stdexcept>
#include <signal.h>
#include <iostream>

CgiHandler::CgiHandler(void) :	pid_(-1),
								cgiPath_(""),
								pipe_(),
								env_() {
	pipe_[0] = -1;
	pipe_[1] = -1;
	this->env_.push_back(NULL);
}

CgiHandler::CgiHandler(std::string const& cgiPath, std::string const& cgiExecutor) :	pid_(-1),
																						cgiPath_(cgiPath),
																						cgiExecutor_(cgiExecutor),
																						pipe_(),
																						env_() {
	pipe_[0] = -1;
	pipe_[1] = -1;
	this->env_.push_back(NULL);
}

CgiHandler::CgiHandler(std::string const& cgiPath,
						std::vector<char*> const& env) :	pid_(-1),
															cgiPath_(cgiPath),
															pipe_(),
															env_(env) {
	pipe_[0] = -1;
	pipe_[1] = -1;
	this->env_.push_back(NULL);
}

CgiHandler::CgiHandler(CgiHandler const& other) :	pid_(other.pid_),
													cgiPath_(other.cgiPath_),
													env_(other.env_) {
	pipe_[0] = other.pipe_[0];
	pipe_[1] = other.pipe_[1];
	this->env_.push_back(NULL);
}

CgiHandler&	CgiHandler::operator=(CgiHandler const& other) {
	if (this != &other) {
		pid_ = other.pid_;
		cgiPath_ = other.cgiPath_;
		pipe_[0] = other.pipe_[0];
		pipe_[1] = other.pipe_[1];
		env_ = other.env_;
	}
	return (*this);
}

CgiHandler::~CgiHandler(void) {
	if (pipe_[0] != -1) {
		close(pipe_[0]);
	}
	if (pipe_[1] != -1) {
		close(pipe_[1]);
	}
	// kill(pid_, SIGKILL);
}

pid_t const&	CgiHandler::getPid(void) const {
	return (pid_);
}

int	CgiHandler::getPipeReadFd(void) const {
	return (this->pipe_[0]);
}

int	CgiHandler::getPipeWriteFd(void) const {
	return (pipe_[1]);
}

std::vector<char*> const&	CgiHandler::getEnv(void) const {
	return (env_);
}

std::string	const&	CgiHandler::getCgiPath(void) const {
	return (cgiPath_);
}

void	CgiHandler::setCgiPath(std::string const& path) {
	this->cgiPath_ = path;
}

void	CgiHandler::addEnvEntry(std::string const& envEntry) {
	std::vector<char*>::iterator it = this->env_.begin();
	std::vector<char*>::iterator ite = this->env_.end();
	for (; it != ite; it++) {
		if (*it == NULL) {
			*it = const_cast<char*>(envEntry.c_str());
			break;
		}
	}
	this->env_.push_back(NULL);
}

void	CgiHandler::executeCgi(void) {
	if (pipe(this->pipe_) == -1) {
		throw std::runtime_error("pipe error");
	}
	std::vector<char*> args;
	if (this->cgiExecutor_.empty()) {
		args.push_back(const_cast<char*>(this->cgiExecutor_.c_str()));
	}
	args.push_back(const_cast<char*>(this->cgiPath_.c_str()));
	args.push_back(NULL);

	this->env_.push_back(NULL);

	this->pid_ = fork();
	if (this->pid_ == -1) {
		throw std::runtime_error("fork error");
	}

	if (this->pid_ == 0) {
		dup2(this->pipe_[0], STDIN_FILENO);
		dup2(this->pipe_[1], STDOUT_FILENO);
		close(this->pipe_[1]);
		close(this->pipe_[0]);
		if (*(this->env_.end()) != NULL) {
			this->env_.push_back(NULL);
		}
		if (execve(cgiPath_.c_str(), args.data(), this->env_.data()) < 0) {
			std::exit(0);
		}
	}
	close(this->pipe_[1]);
	this->pipe_[1] = -1;
}
