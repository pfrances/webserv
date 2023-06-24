/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:38:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:23:36 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQ_HPP
# define POST_REQ_HPP

# include "Request.hpp"
class Server;

class PostReq : public Request {
	public:
		PostReq(std::string const& rawRequest);
		PostReq(PostReq const& other);
		PostReq &operator=(PostReq const& other);
		virtual ~PostReq(void);

		virtual Response*	execute(ServerMonitor& serverMonitor) const;

	private:
		PostReq(void);
		virtual bool	isValidReq(void) const;
};

#endif
