/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef UPTIME_H
#define UPTIME_H

class Uptime {
	private:
		Uptime(const Uptime &copy);
#ifdef TIMER_S
		uint16_t seconds;
#endif

	public:
#ifdef TIMER_S
		Uptime () : seconds(0) {}
#else
		Uptime () {}
#endif
		inline uint16_t get_us() { return 0; }
		inline uint16_t get_cycles() { return 0; }
#ifdef TIMER_S
		inline uint16_t get_s() { return seconds; }
		inline void tick_s() { seconds++; }
#endif
};

extern Uptime uptime;

#endif
