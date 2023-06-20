/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetReq.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:37:43 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:38:00 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_REQ_HPP
# define GET_REQ_HPP

# include "Request.hpp"

class GetReq : public Request {
	public:
		GetReq(void);
		GetReq(std::string rawRequest);
		GetReq(const GetReq &other);
		GetReq &operator=(const GetReq &other);
		virtual ~GetReq(void);
};

#endif