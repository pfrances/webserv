/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:45:00 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 16:16:35 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include <string>
# include <vector>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "Location.hpp"
# include "Request.hpp"

class CgiHandler {
	public:
		CgiHandler(std::string const& cgiPath, std::string const& cgiExecutor);
		CgiHandler(CgiHandler const& other);
		CgiHandler &operator=(CgiHandler const& other);
		~CgiHandler(void);

		void							executeCgi(void);

		pid_t const&					getPid(void) const;
		int								getPipeReadFd(void) const;
		int								getPipeWriteFd(void) const;
		std::vector<std::string> const&	getEnv(void) const;
		std::string	const&				getCgiPath(void) const;
		int								getClientFd(void) const;
		double							getStartTime(void) const;
		std::string const&				getCookieId(void) const;

		bool							isCookieIdSet(void) const;

		void							setCgiPath(std::string const& path);
		void							setClientFd(int fd);
		void							setStartTime(double time);
		void							setEnvKey(std::string const& key, std::string const& value);
		void							setEnv(Request const& req);
		void							setCookieId(std::string const& cookieId);
		void							writeBodyToCgiStdin(void);
		void							setCookieIdIsNew(bool isNew);

	private:
		CgiHandler(void);
		pid_t						pid_;
		std::string					cgiPath_;
		std::string					cgiExecutor_;
		int							pipe_[2];
		std::vector<std::string>	envVec_;
		std::vector<std::string>	argsVec_;
		int							clientFd_;
		double						startTime_;
		std::string					body_;
		std::string					cookieId_;
		bool						cookieIdIsNew_;

		char *const*				StringVecToCharPtrArray(std::vector<std::string> const& vec) const;
};

#endif
