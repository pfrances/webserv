/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:20:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/29 13:13:16 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HttpMessage.hpp"
# include "CgiHandler.hpp"

class Response : public HttpMessage {
	public:
		Response(void);
		Response(std::string const& rawResponse);
		Response(Response const& other);
		Response &operator=(Response const& other);
		virtual ~Response(void);

		std::string const&	getStatusCode(void) const;
		std::string const&	getStatusMessage(void) const;
		std::string const&	getHttpVersion(void) const;
		CgiHandler*			getCgiHandler(void);
		int					getClientFd(void) const;

		void				setStatusCode(std::string const& statusCode);
		void				setStatusCode(int statusCode);
		void				setStatusMessage(std::string const& statusMessage);
		void				setStatusMessageFromCode(int statusCode);
		void				setHttpVersion(std::string const& httpVersion);
		void				setCgiHandler(std::string const& path, std::string const& cgiExecutor);
		void				setClientFd(int fd);

		bool				hasCgiHandler(void) const;
		void				killCgiHandler(void);


	private:
		virtual	void		parseStartLine(void);
		virtual void		updateStartLine(void);

		std::string			statusCode_;
		std::string			statusMessage_;
		std::string			httpVersion_;

		CgiHandler			*cgiHandler_;
		int					clientFd_;
};

#endif
