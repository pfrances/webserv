/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 14:42:05 by pfrances          #+#    #+#             */
/*   Updated: 2023/06/17 14:42:30 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ServerMonitor.hpp"
#include <stdexcept>

int	main(void)
{
	try {
		ServerMonitor webserv;
		webserv.run();
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
