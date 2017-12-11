#include "driver/uptime.h"
#include <time.h>

uint64_t Uptime::get()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_nsec + (1000000ULL * ((uint64_t)ts.tv_sec % 256));
}

Uptime uptime;
