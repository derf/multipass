#ifndef UPTIME_H
#define UPTIME_H

#include <avr/io.h>

class Uptime {
	private:
		Uptime(const Uptime &copy);
#ifdef TIMER_S
		uint8_t seconds;
#endif

	public:
#ifdef TIMER_S
		Uptime () : seconds(0) {}
#else
		Uptime () {}
#endif
		inline uint8_t get_cycles() { return TCNT0; }
#ifdef TIMER_S
		inline uint8_t get_s() { return seconds; }
		inline void tick_s() { seconds++; }
#endif
};

extern Uptime uptime;

#endif
