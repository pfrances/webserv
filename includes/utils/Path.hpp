/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Path.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 11:32:28 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/20 11:52:04 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_HPP
# define PATH_HPP

# include <string>
# include <list>

class Path
{
	public:
		Path(void);
		Path(std::string const& path);
		Path(Path const& other);
		Path &operator=(const Path &other);
		~Path(void);

		std::string const&				getFullPath(void) const;
		std::string const&				getRelativePath(void) const;
		std::list<std::string> const&	getFullPathList(void) const;
		std::list<std::string> const&	getRelativePathList(void) const;

		std::string						getPathFrom(std::string const& path) const;
		std::string						getPathTo(std::string const& path) const;

		void							setPath(std::string const& path);

	private:
		std::string				absolutePath_;
		std::string				relativePath_;
		std::list<std::string>	absolutePathList_;
		std::list<std::string>	relativePathList_;

};

#endif
