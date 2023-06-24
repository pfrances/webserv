/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteReq.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:36:26 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 14:23:36 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETE_REQ_HPP
# define DELETE_REQ_HPP

# include "Request.hpp"
class Server;

class DeleteReq : public Request {
	public:
		DeleteReq(std::string const& rawRequest);
		DeleteReq(DeleteReq const& other);
		DeleteReq &operator=(DeleteReq const& other);
		virtual ~DeleteReq(void);

		virtual Response*	execute(ServerMonitor& serverMonitor) const;

	private:
		DeleteReq(void);
		virtual bool	isValidReq(void) const;
};

#endif
