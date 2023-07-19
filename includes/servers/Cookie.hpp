/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:00:55 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 14:30:51 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COOKIE_HPP
# define COOKIE_HPP

# define COOKIE_EXPIRE_TIME 3600
# include <string>

class Cookie {
	public:
		Cookie(void);
		Cookie(Cookie const& other);
		Cookie &operator=(Cookie const& other);
		~Cookie(void);

		std::string const&		getCookieId(void) const;
		std::string const&		getContent(void) const;

		void					setCookieId(std::string const& cookieId);
		void					setContent(std::string const& content);
		void					addContent(std::string const& content);
		void					updateExpireTime(void);
		bool					isExpired(void) const;

	private:
		std::string			cookieId_;
		std::string			content;
		double				expireTime_;

		void				generateCookieId(void);
};

#endif
