/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:20:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:23:36 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HttpMessage.hpp"

class Server;

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

		void				setStatusCode(std::string const& statusCode);
		void				setStatusMessage(std::string const& statusMessage);
		void				setHttpVersion(std::string const& httpVersion);

	protected:
		std::string			statusCode_;
		std::string			statusMessage_;
		std::string			httpVersion_;

	private:
		virtual	void		parseStartLine(void);
		virtual void		updateStartLine(void);
};

#endif
