#ifndef UPTIME_H
#define UPTIME_H

#include <msp430.h>
#include <stdint.h>

class Uptime {
	private:
		Uptime(const Uptime &copy);
#ifdef TIMER_S
		uint16_t seconds;
#endif

	public:
		Uptime () : seconds(0) {}
		inline uint16_t get_us() { return TA0R; }
		inline uint16_t get_cycles() { return TA2R; }
#ifdef TIMER_S
		inline uint16_t get_s() { return seconds; }
		inline void tick_s() { seconds++; }
#endif
};

extern Uptime uptime;

#endif
