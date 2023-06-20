/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directory.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 12:10:19 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/20 12:21:19 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTORY_HPP
# define DIRECTORY_HPP

# include "Path.hpp"
# include <vector>

class File;

class Directory : public Path
{
	public:
		Directory(std::string const& path);
		Directory(Directory const& other);
		Directory &operator=(Directory const& other);
		~Directory(void);

		bool						isHidden(void) const;

		std::string const&			getFileName(void) const;
		std::string const&			getFileBaseName(void) const;
		std::string const&			getExtension(void) const;
		std::string const&			getFileContent(void) const;

		std::vector<File> const&	getFilesInDir(void) const;

	private:
		Directory(void);
		bool				isHidden_;
		std::vector<File>	filesInDir_;
};

#endif
