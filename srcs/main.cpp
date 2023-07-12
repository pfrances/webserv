/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 14:42:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/12 12:11:57 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerMonitor.hpp"
#include <iostream>
#include <stdexcept>
#include <csignal>
#include <cstdlib>

void signalHandler(int signal) {
	if (signal == SIGINT) {
		throw std::runtime_error("\nTerminated by SIGINT");
	}
}

int	main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " [path/to/config_file]." << std::endl;
		return (1);
	}
	signal(SIGINT, signalHandler);

	try {
		ServerMonitor webserv(argv[1]);
		webserv.run();
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
