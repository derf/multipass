#include "arch.h"
#include "driver/counter.h"
#include "driver/gpio.h"

#if defined(TIMER_CYCLES)
#warn "timer_cycles and counter are mutually exclusive. Expect odd behaviour."
#endif

Counter counter;

#ifndef __acweaving
__attribute__((interrupt(TIMER2_A1_VECTOR))) void handle_timer2_overflow()
{
	if (TA2IV == 0x0e) {
		if (counter.overflow < 255) {
			counter.overflow++;
		}
	}
}
#endif
