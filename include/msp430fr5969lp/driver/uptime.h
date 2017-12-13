#ifndef UPTIME_H
#define UPTIME_H

#include <msp430.h>
#include <stdint.h>

class Uptime {
	private:
		Uptime(const Uptime &copy);

	public:
		Uptime () {}
		inline uint16_t get_us() { return TA0R; }
		inline uint16_t get_s() { return 0; }
		inline uint16_t get_cycles() { return TA2R; }
};

extern Uptime uptime;

#endif
