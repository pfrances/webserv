/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:20:33 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/24 13:25:09 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>

class Location {
public:
	Location(void);
	Location(std::string const& locationBlock, std::string const& path);
	Location(Location const& other);
	Location &operator=(Location const& other);
	~Location(void);

	std::string const&							getPath(void) const;
	std::string const&							getRoot(void) const;
	std::string const&							getUploadPath(void) const;
	std::vector<std::string> const&				getIndex(void) const;
	std::vector<std::string> const&				getTryFiles(void) const;
	std::map<int, std::string> const&			getErrorPages(void) const;
	std::map<std::string, std::string> const&	getRedirect(void) const;
	std::vector<std::string> const&				getAllowedMethods(void) const;
	std::vector<std::string> const&				getCgiPath(void) const;
	std::vector<std::string> const&				getCgiExtension(void) const;
	size_t										getClientMaxBodySize(void) const;
	bool										getDirectoryListing(void) const;

	void		setPath(std::string const& path);

	void		setRoot(std::string const& root);

	void		setUploadPath(std::string const& uploadPath);

	void		setIndex(std::vector<std::string> const& index);
	void		addIndex(std::string const& index);

	void		setTryFiles(std::vector<std::string> const& tryFiles);
	void		addTryFiles(std::string const& tryFiles);

	void		setErrorPages(std::map<int, std::string> const& errorPages);
	void		addErrorPages(int errorCode, std::string const& errorPage);
	void		parseAndAddErrorPages(std::vector<std::string> const& errorPages);

	void		setRedirect(std::map<std::string, std::string> const& redirect);
	void		addRedirect(std::string const& from, std::string const& to);

	void		setAllowedMethods(std::vector<std::string> const& allowedMethods);
	void		addAllowedMethods(std::string const& allowedMethods);

	void		setCgiPath(std::vector<std::string> const& cgiPath);
	void		addCgiPath(std::string const& cgiPath);

	void		setCgiExtension(std::vector<std::string> const& cgiExtension);
	void		addCgiExtension(std::string const& cgiExtension);

	void		setClientMaxBodySize(size_t clientMaxBodySize);
	void		setClientMaxBodySize(std::string const& clientMaxBodySize);

	void		setDirectoryListing(bool directoryListing);

	void		applyDefaultValues(Location const& defaultLocation);

private:
	void parseLocationConf(std::string const& locationConfig);

	std::string							path_;
	std::string							root_;

	std::string							uploadPath_;

 	std::vector<std::string>			index_;
	std::vector<std::string>			tryFiles_;
 	std::map<int, std::string>			errorPages_;

	std::map<std::string, std::string>	redirect_;

	std::vector<std::string>			allowedMethods_;

	std::vector<std::string>			cgiPath_;
	std::vector<std::string>			cgiExtension_;

	size_t								clientMaxBodySize_;
	bool								directoryListing_;
};

#endif