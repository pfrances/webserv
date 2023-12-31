/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:20:33 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/18 14:45:37 by pfrances         ###   ########.fr       */
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
	std::map<int, std::string> const&			getErrorPages(void) const;
	std::string const&							getRedirectUri(void) const;
	int											getRedirectStatusCode(void) const;
	std::vector<std::string> const&				getAllowedMethods(void) const;
	std::vector<std::string> const&				getCgiExecutor(void) const;
	std::vector<std::string> const&				getCgiExtensions(void) const;
	std::string const&							getCgiExecutorByExtension(std::string const& extension) const;
	size_t										getClientMaxBodySize(void) const;
	bool										getAutoIndex(void) const;

	bool										isGetAllowed(void) const;
	bool										isPostAllowed(void) const;
	bool										isDeleteAllowed(void) const;
	bool										isCgiLocation(void) const;


	void		setPath(std::string const& path);

	void		setRoot(std::string const& root);

	void		setUploadPath(std::string const& uploadPath);

	void		setIndex(std::vector<std::string> const& index);
	void		addIndex(std::string const& index);

	void		setErrorPages(std::map<int, std::string> const& errorPages);
	void		addErrorPages(int errorCode, std::string const& errorPage);
	void		parseAndAddErrorPages(std::vector<std::string> const& errorPages);

	void		setRedirectStatusCode(int statusCode);
	void		setRedirectUri(std::string const& redirectUri);

	void		setAllowedMethods(std::vector<std::string> const& allowedMethods);
	void		addAllowedMethods(std::string const& allowedMethods);

	void		setCgiExecutor(std::vector<std::string> const& CgiExecutors);
	void		addCgiExecutor(std::string const& CgiExecutor);

	void		setCgiExtension(std::vector<std::string> const& cgiExtension);
	void		addCgiExtension(std::string const& cgiExtension);

	void		setClientMaxBodySize(size_t clientMaxBodySize);
	void		setClientMaxBodySize(std::string const& clientMaxBodySize);

	void		setAutoIndex(bool directoryListing);

	void		applyDefaultValues(Location const& defaultLocation);


private:
	void		parseLocationConf(std::string const& locationConfig);

	std::string							path_;
	std::string							root_;

	std::string							uploadPath_;

 	std::vector<std::string>			index_;
 	std::map<int, std::string>			errorPages_;

	int									redirectStatusCode_;
	std::string							redirectUri_;

	std::vector<std::string>			allowedMethods_;

	std::vector<std::string>			cgiExecutor_;
	std::vector<std::string>			cgiExtensions_;

	size_t								clientMaxBodySize_;
	bool								autoIndex_;
};

#endif
