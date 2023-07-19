/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:44:51 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 11:50:38 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "ParseTools.hpp"
#include <stdexcept>
#include <cstdlib>
#include <signal.h>
#include <iostream>

CgiHandler::CgiHandler(void) : pid_(-1), cgiPath_(""), cgiExecutor_(""), pipe_(), envVec_(), argsVec_() {
	this->pipe_[0] = -1;
	this->pipe_[1] = -1;
}

CgiHandler::CgiHandler(std::string const& cgiPath, std::string const& cgiExecutor)
	: pid_(-1), cgiPath_(cgiPath), cgiExecutor_(cgiExecutor), pipe_(), envVec_(), argsVec_() {
	this->pipe_[0] = -1;
	this->pipe_[1] = -1;
	if (!cgiExecutor_.empty()) {
		this->argsVec_.push_back(cgiExecutor_);
	}
	this->argsVec_.push_back(cgiPath);
}

CgiHandler::CgiHandler(CgiHandler const& other)
	: pid_(other.pid_), cgiPath_(other.cgiPath_), cgiExecutor_(other.cgiExecutor_), envVec_(other.envVec_), argsVec_(other.argsVec_) {
	this->pipe_[0] = other.pipe_[0];
	this->pipe_[1] = other.pipe_[1];
}

CgiHandler &CgiHandler::operator=(CgiHandler const& other) {
	if (this != &other) {
		this->pid_ = other.pid_;
		this->cgiPath_ = other.cgiPath_;
		this->cgiExecutor_ = other.cgiExecutor_;
		this->pipe_[0] = other.pipe_[0];
		this->pipe_[1] = other.pipe_[1];
		this->envVec_ = other.envVec_;
		this->argsVec_ = other.argsVec_;
	}
	return *this;
}

CgiHandler::~CgiHandler(void) {
	if (this->pipe_[0] != -1) {
		close(this->pipe_[0]);
	}
	if (this->pipe_[1] != -1) {
		close(this->pipe_[1]);
	}
	if (this->pid_ > 0) {
		kill(this->pid_, SIGKILL);
		waitpid(this->pid_, NULL, 0);
	}
}

pid_t const& CgiHandler::getPid(void) const {
	return this->pid_;
}

int CgiHandler::getPipeReadFd(void) const {
	return this->pipe_[0];
}

int CgiHandler::getPipeWriteFd(void) const {
	return this->pipe_[1];
}

std::vector<std::string> const& CgiHandler::getEnv(void) const {
	return this->envVec_;
}

std::string const& CgiHandler::getCgiPath(void) const {
	return this->cgiPath_;
}

int CgiHandler::getClientFd(void) const {
	return this->clientFd_;
}

double CgiHandler::getStartTime(void) const {
	return this->startTime_;
}

void CgiHandler::setCgiPath(std::string const& path) {
	this->cgiPath_ = path;
}

void	CgiHandler::setClientFd(int fd) {
	this->clientFd_ = fd;
}

void	CgiHandler::setStartTime(double time) {
	this->startTime_ = time;
}

void	CgiHandler::setEnvKey(std::string const& key, std::string const& value) {
	this->envVec_.push_back(key + "=" + value);
}

void	CgiHandler::setEnv(Request const& req) {
	this->setEnvKey("SERVER_PROTOCOL", req.getHttpVersion());
	this->setEnvKey("REQUEST_METHOD", req.getMethod());
	this->setEnvKey("PATH_INFO", ".");
	this->setEnvKey("SERVER_NAME", req.getHostName());
	this->setEnvKey("QUERY_STRING", req.getQueryStr());
	this->setEnvKey("CONTENT_TYPE", req.getSingleHeader("Content-Type"));
	this->setEnvKey("SCRIPT_NAME", this->cgiPath_);
	std::string const& body = req.getBody();
	if (!req.getBoundary().empty()) {
		this->body_ = ParseTools::parseBoundaryBody(body, req.getBoundary());
	} else if (req.getSingleHeader("Content-Type").find("application/x-www-form-urlencoded") != std::string::npos) {
		this->body_ = ParseTools::parseUrlEncodedBody(body);
	} else {
		this->body_ = body;
	}
	this->setEnvKey("CONTENT_LENGTH", ParseTools::intToString(this->body_.length()));
}

void	CgiHandler::writeBodyToCgiStdin(void) {
	if (!this->body_.empty() && this->pipe_[1] > 0 && !this->body_.empty()) {
		write(this->pipe_[1], this->body_.data(), this->body_.length());
	}
	if (this->pipe_[1] > 0) {
		close(this->pipe_[1]);
		this->pipe_[1] = -1;
	}
}

void CgiHandler::executeCgi(void) {
	if (pipe(this->pipe_) < 0) {
		throw std::runtime_error("pipe error");
	}

	this->pid_ = fork();
	if (this->pid_ < 0) {
		throw std::runtime_error("fork error");
	}

	if (this->pid_ == 0) {
		// close(STDERR_FILENO);
		dup2(this->pipe_[0], STDIN_FILENO);
		dup2(this->pipe_[1], STDOUT_FILENO);
		close(this->pipe_[1]);
		close(this->pipe_[0]);
		char *const *args = StringVecToCharPtrArray(this->argsVec_);
		char *const *env = StringVecToCharPtrArray(this->envVec_);

		if (execve(args[0], args, env) < 0) {
			delete[] args;
			delete[] env;
			throw std::runtime_error("execve error");
		}
	}
	if (this->body_.empty()) {
		close(this->pipe_[1]);
		this->pipe_[1] = -1;
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
