/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 19:02:03 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 20:36:08 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HttpMessage.hpp"
# include <vector>

class ServerMonitor;
class Response;
class Server;

class Request : public HttpMessage {
	public:
		Request(void);
		Request(std::string const& rawRequest);
		Request(Request const& other);
		Request &operator=(Request const& other);
		virtual ~Request(void);

		std::string const&	getMethod(void) const;
		std::string const&	getUri(void) const;
		std::string const&	getHttpVersion(void) const;

		void				setMethod(std::string const& method);
		void				setUri(std::string const& uri);
		void				setHttpVersion(std::string const& httpVersion);

		virtual Response*	execute(ServerMonitor& serverMonitor) const = 0;

	protected:
		std::string			method_;
		std::string			uri_;
		std::string			httpVersion_;
		//std::vector<pollfd>	pendingFds_;
		virtual bool		isValidReq(void) const = 0;

	private:
		virtual	void		parseStartLine(void);
		virtual void		updateStartLine(void);
};

Request*	createRequestFromRequestString(std::string const& rawRequest);

#endif
