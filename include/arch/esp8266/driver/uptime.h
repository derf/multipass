#ifndef UPTIME_H
#define UPTIME_H

extern "C" {
#include "osapi.h"
#include "user_interface.h"
}
#include "c_types.h"

class Uptime {
	private:
		Uptime(const Uptime &copy);

	public:
		Uptime () {}
		inline uint32_t get_us() { return system_get_time(); }

		inline uint32_t get_cycles()
		{
			uint32_t ccount;
			asm volatile ("esync; rsr %0,ccount":"=a" (ccount));
			return ccount;
		}
};

extern Uptime uptime;

#endif
