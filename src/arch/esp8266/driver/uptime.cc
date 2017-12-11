#include "driver/uptime.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
}

uint32_t Uptime::get()
{
	return system_get_time();
}

Uptime uptime;
