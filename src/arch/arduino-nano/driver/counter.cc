#include "driver/counter.h"

#if defined(TIMER_S) || defined(WITH_LOOP)
#warn "timer/loop and counter are mutually exclusive. Expect odd behaviour."
#endif

Counter counter;

ISR(TIMER1_OVF_vect)
{
	if (counter.overflowed < 255) {
		counter.overflowed++;
	}
}
