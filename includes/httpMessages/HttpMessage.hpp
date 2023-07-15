/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 12:09:28 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/15 15:21:31 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP

# define CRLF "\r\n" // Carriage Return & Line Feed

# include <string>
# include <map>

class HttpMessage
{
	public:
		HttpMessage(void);
		HttpMessage(std::string const& rawMessage);
		HttpMessage& operator=(HttpMessage const& other);
		HttpMessage(HttpMessage const& other);
		virtual ~HttpMessage(void);


		void							setStartLine(std::string const& firstLine);
		void							setSingleHeader(std::string const& key, std::string const& value);
		void							setHeadersStr(std::string const& headersStr);
		void							setHeadersMap(std::map<std::string, std::string> const& headers);
		void							setBody(std::string const& body);
		void							appendToBody(std::string const& chunk);
		void							setRawMessage(std::string const& rawMessage);

		std::string const&				getStartLine(void) const;
		std::string const&				getSingleHeader(std::string const& key) const;
		std::map<std::string,
				std::string>const&		getHeadersMap(void) const;
		std::string const&				getHeadersStr(void) const;
		std::string const&				getBody(void) const;
		std::string const&				getRawMessage(void) const;
		std::string const&				getHostName(void) const;
		std::string const&				getBoundary(void) const;

		bool							isFetched(void) const;

		bool							hasStartLine(void) const;
		bool							hasHeaders(void) const;
		bool							hasHeaderEntry(std::string const& key) const;
		bool							hasBody(void) const;
		bool							hasRawMessage(void) const;

		void							setMimeByExtension(std::string const& extension);

	protected:
		std::string						rawMessage_;
		std::string						startLine_;
		std::map<std::string,
				std::string>			headersMap_;
		std::string						headersStr_;
		std::string						body_;

		bool							chunksFetched_;
		size_t							totalSize_;
		std::string						boundary_;
		std::string						hostName_;

		virtual void					parseStartLine(void) = 0;
		virtual void					updateStartLine(void) = 0;
		void							updateRawMessage(void);

	private:
		void							parseRawMessage(void);
		void							parseHeadersMap(void);
		std::pair<std::string,
				std::string>			parseSingleHeader(std::string const& header);
		void							parseBody(std::string const& body);

		void							updateHeadersStr(void);
};

#endif
