/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef UPTIME_H
#define UPTIME_H

class Uptime {
	private:
		Uptime(const Uptime &copy);
#ifdef TIMER_S
		unsigned int seconds;
#endif

	public:
#ifdef TIMER_S
		Uptime () : seconds(0) {}
#else
		Uptime () {}
#endif
#ifdef TIMER_S
		inline unsigned int get_s() { return seconds; }
		inline void tick_s() { seconds++; }
#endif
};

extern Uptime uptime;

#endif
