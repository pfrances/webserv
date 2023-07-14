/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:45:00 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 19:36:31 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include <string>
# include <vector>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

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
		size_t							getStartTime(void) const;

		void							setCgiPath(std::string const& path);
		void							setClientFd(int fd);
		void							setStartTime(size_t time);

	private:
		CgiHandler(void);
		pid_t						pid_;
		std::string					cgiPath_;
		std::string					cgiExecutor_;
		int							pipe_[2];
		std::vector<std::string>	envVec_;
		std::vector<std::string>	argsVec_;
		int							clientFd_;
		size_t						startTime_;

		char *const*				StringVecToCharPtrArray(std::vector<std::string> const& vec) const;
};

#endif
