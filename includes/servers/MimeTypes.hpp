/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 14:21:20 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/29 15:47:39 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MIMETYPES_HPP
# define MIMETYPES_HPP

# include <string>
# include <map>

class MimeTypes
{
	public:
		static std::string const&		getMimeType(std::string const& extension);

	private:
		static void						initMimeTypes(void);

		static bool						isInit_;
		static std::map<std::string,
						std::string>	mimeTypes_;
};

#endif
