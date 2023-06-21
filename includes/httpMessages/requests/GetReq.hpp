/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetReq.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:37:43 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 13:18:40 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_REQ_HPP
# define GET_REQ_HPP

# include "Request.hpp"

class ServConf;
class Response;

class GetReq : public Request {
	public:
		GetReq(std::string const& rawRequest);
		GetReq(GetReq const& other);
		GetReq &operator=(GetReq const& other);
		virtual ~GetReq(void);

		virtual Response*	execute(ServConf const& conf) const;

	private:
		GetReq(void);
		virtual bool	isValidReq(void) const;

};

#endif
