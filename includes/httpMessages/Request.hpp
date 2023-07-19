/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 19:02:03 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 15:35:45 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HttpMessage.hpp"
# include <vector>

class Request : public HttpMessage {
	public:
		Request(void);
		Request(std::string const& rawRequest);
		Request(Request const& other);
		Request &operator=(Request const& other);
		virtual ~Request(void);

		std::string const&		getMethod(void) const;
		std::string const&		getUri(void) const;
		std::string const&		getUriWithQuery(void) const;
		std::map<std::string,
			std::string> const&	getQuery(void) const;
		std::string const&		getQueryStr(void) const;
		std::string const&		getHttpVersion(void) const;
		std::string				getCookieValue(std::string const& key) const;

		void					setMethod(std::string const& method);
		void					setUri(std::string const& uri);
		void					setQuery(std::string const& querryStr);
		void					setHttpVersion(std::string const& httpVersion);

		bool					isMethodValid(void) const;
		bool					isUriValid(void) const;
		bool					isHttpVersionValid(void) const;
		virtual bool			isValid(void) const;

	private:
		virtual	void			parseStartLine(void);
		virtual void			updateStartLine(void);

		std::string				method_;
		std::string				uri_;
		std::map<std::string,
				 std::string>	query_;
		std::string				queryStr_;
		std::string				uriWithQuery_;

		std::string				httpVersion_;
};

std::vector<Request*> parseMultipleRequest(std::string const& allMsgs);

#endif
