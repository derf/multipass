#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

class Uptime {
	private:
		Uptime(const Uptime &copy);

	public:
		Uptime () {}
		uint32_t get();
};

extern Uptime uptime;

#endif
