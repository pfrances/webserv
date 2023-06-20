/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 10:20:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:40:09 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HttpMessage.hpp"

class ServConf;

class Response : public HttpMessage {
	public:
		Response(void);
		Response(std::string rawResponse);
		Response(const Response &other);
		Response &operator=(const Response &other);
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
