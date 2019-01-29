#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

class Uptime {
	private:
		Uptime(const Uptime &copy);

	public:
		Uptime () {}
		uint64_t get_s();
		uint64_t get_us();
		uint64_t get_cycles();
		void tick_s() {};
};

extern Uptime uptime;

#endif
