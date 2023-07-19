/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 19:28:38 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/19 14:25:26 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMER_HPP
# define TIMER_HPP

#include <ctime>
#include <stdio.h>

class Timer {
	public:
		Timer(void);
		Timer(Timer const& other);
		Timer &operator=(Timer const& other);
		~Timer(void);

		void			startTimer(void);
		double			getElapsedTimeSince(std::time_t time) const;
		double			getElapsedTimeSince(void) const;
		void			printLogTime(void) const;
		static double	getCurrentTime(void);

	private:
		std::time_t	startTime_;
};

#endif