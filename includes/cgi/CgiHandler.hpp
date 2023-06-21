/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/19 19:45:00 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/21 14:44:16 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

class CgiHandler {
	public:
		CgiHandler(void);
		CgiHandler(CgiHandler const& other);
		CgiHandler &operator=(CgiHandler const& other);
		virtual ~CgiHandler(void);
};

#endif
