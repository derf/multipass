#include "driver/uptime.h"
#include <msp430.h>

uint32_t Uptime::get()
{
	return TA0R;
}

Uptime uptime;
