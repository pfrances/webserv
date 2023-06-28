/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 19:02:03 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/28 16:29:59 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HttpMessage.hpp"

class Request : public HttpMessage {
	public:
		Request(std::string const& rawRequest, int clientSocket);
		Request(Request const& other);
		Request &operator=(Request const& other);
		virtual ~Request(void);

		std::string const&		getMethod(void) const;
		std::string const&		getUri(void) const;
		std::string const&		getHttpVersion(void) const;

		void					setMethod(std::string const& method);
		void					setUri(std::string const& uri);
		void					setQuery(std::string const& querryStr);
		void					setHttpVersion(std::string const& httpVersion);

	private:
		Request(void);
		virtual	void			parseStartLine(void);
		virtual void			updateStartLine(void);

		std::string				method_;
		std::string				uri_;
		std::map<std::string,
				 std::string>	query_;

		std::string				httpVersion_;

		int						clientSocket_;
};

#endif