/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Timer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pfrances <pfrances@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 19:28:38 by pfrances          #+#    #+#             */
/*   Updated: 2023/07/13 19:43:01 by pfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIMER_HPP
# define TIMER_HPP

#include <sys/time.h>
#include <stdio.h>

class Timer {
	public:
		Timer(void);
		Timer(Timer const& other);
		Timer &operator=(Timer const& other);
		~Timer(void);

		void			startTimer(void);
		size_t			getCurrentTime(void) const;
		size_t			getElapsedTimeSince(size_t time) const;

	private:
		struct timeval		startTime_;
};

#endif