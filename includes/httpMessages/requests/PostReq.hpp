/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:38:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 13:18:31 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQ_HPP
# define POST_REQ_HPP

# include "Request.hpp"
class ServConf;

class PostReq : public Request {
	public:
		PostReq(std::string const& rawRequest);
		PostReq(PostReq const& other);
		PostReq &operator=(PostReq const& other);
		virtual ~PostReq(void);

		virtual Response*	execute(ServConf const& conf) const;

	private:
		PostReq(void);
		virtual bool	isValidReq(void) const;
};

#endif
