/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 19:02:03 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:49:27 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HttpMessage.hpp"
# include "Response.hpp"
// class Response; 
class ServConf;

class Request : public HttpMessage {
	public:
		Request(void);
		Request(std::string rawRequest);
		Request(const Request &other);
		Request &operator=(const Request &other);
		virtual ~Request(void);

		std::string const&	getMethod(void) const;
		std::string const&	getUri(void) const;
		std::string const&	getHttpVersion(void) const;
		Response const&		generateResponse(ServConf const& conf);

		void				setMethod(std::string const& method);
		void				setUri(std::string const& uri);
		void				setHttpVersion(std::string const& httpVersion);

		virtual bool		isValidReq(void) const = 0;

	protected:
		std::string			method_;
		std::string			uri_;
		std::string			httpVersion_;
		Response			res_;
		virtual void		process(ServConf conf) = 0;

	private:
		virtual	void	parseStartLine(void);
		virtual void	updateStartLine(void);
};

Request*	createRequestFromRequestString(std::string const& rawRequest);

#endif
