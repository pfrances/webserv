/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 11:56:13 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 15:08:41 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

# include "Path.hpp"

class Directory;

class File : public Path
{
	public:
		File(void);
		File(std::string const& path);
		File(File const& other);
		File &operator=(File const& other);
		~File(void);

		bool				exists(void) const;
		bool				isExecutable(void) const;
		bool				isReadable(void) const;
		bool				isWritable(void) const;
		bool				isHidden(void) const;

		std::string const&	getFileName(void) const;
		std::string const&	getFileBaseName(void) const;
		std::string const&	getExtension(void) const;
		std::string const&	getFileContent(void);

		void				setFileName(std::string const& fileName);
		void				setFileBaseName(std::string const& fileBaseName);
		void				setExtension(std::string const& extension);
		void				setFileContent(std::string const& content);
		void				append(std::string const& content);

	private:
		void				read(void);
		void				write(void);
		void				parsePath(void);

		// bool				isExecutable_;
		// bool				isReadable_;
		// bool				isWritable_;
		// bool				isHidden_;

		std::string			fileName_;
		std::string			fileBaseName_;
		std::string			extension_;
		std::string			fileContent_;
};

std::ostream &operator<<(std::ostream &o, File& file);

#endif
