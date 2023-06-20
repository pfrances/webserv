/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteReq.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:36:26 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 19:51:38 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETE_REQ_HPP
# define DELETE_REQ_HPP

# include "Request.hpp"
class ServConf;

class DeleteReq : public Request {
	public:
		DeleteReq(std::string rawRequest);
		DeleteReq(const DeleteReq &other);
		DeleteReq &operator=(const DeleteReq &other);
		virtual ~DeleteReq(void);

	protected:
		virtual bool	isValidReq(void) const;
	
	private:
		DeleteReq(void);
		virtual void	process(ServConf conf);
};

#endif
