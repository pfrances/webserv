/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetReq.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:37:43 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:23:36 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_REQ_HPP
# define GET_REQ_HPP

# include "Request.hpp"

class Server;
class Response;

class GetReq : public Request {
	public:
		GetReq(std::string const& rawRequest);
		GetReq(GetReq const& other);
		GetReq &operator=(GetReq const& other);
		virtual ~GetReq(void);

		virtual Response*	execute(ServerMonitor& serverMonitor) const;

	private:
		GetReq(void);
		virtual bool	isValidReq(void) const;

};

#endif
