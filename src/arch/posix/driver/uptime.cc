#include "driver/uptime.h"
#include <time.h>

uint64_t Uptime::get_s()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_sec;
}

uint64_t Uptime::get_us()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_nsec / 1000;
}

uint64_t Uptime::get_cycles()
{
	struct timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	return ts.tv_nsec;
}

Uptime uptime;
