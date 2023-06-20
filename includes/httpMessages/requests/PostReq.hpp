/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:38:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:38:21 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQ_HPP
# define POST_REQ_HPP

# include "Request.hpp"

class PostReq : public Request {
	public:
		PostReq(void);
		PostReq(std::string rawRequest);
		PostReq(const PostReq &other);
		PostReq &operator=(const PostReq &other);
		virtual ~PostReq(void);
};

#endif