/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 14:42:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/23 15:15:42 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ServerMonitor.hpp"
#include <stdexcept>

int	main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " [path/to/config_file]." << std::endl;
		return (1);
	}
	try {
		ServerMonitor webserv(argv[1]);
		webserv.run();
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
