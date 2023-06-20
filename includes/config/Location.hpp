/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:20:33 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/19 10:21:49 by pfrances         ###   ########.fr       */
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
	~Location(void);
	Location(const Location &other);
	Location &operator=(const Location &other);

	void parseLocationConf(std::string locationConfig);

private:
	std::string							path_;
	std::string							root_;

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
