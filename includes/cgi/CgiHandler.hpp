/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:45:00 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/01 19:37:27 by pfrances         ###   ########.fr       */
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
		CgiHandler(std::string const& cgiPath, std::vector<char*> const& env);
		CgiHandler(CgiHandler const& other);
		CgiHandler &operator=(CgiHandler const& other);
		~CgiHandler(void);

		void						executeCgi(void);

		pid_t const&				getPid(void) const;
		int							getPipeReadFd(void) const;
		int							getPipeWriteFd(void) const;
		std::vector<char*> const&	getEnv(void) const;
		std::string	const&			getCgiPath(void) const;

	private:
		CgiHandler(void);
		pid_t				pid_;
		std::string			cgiPath_;
		std::string			cgiExecutor_;
		int					pipe_[2];
		std::vector<char*>	env_;

};

#endif
