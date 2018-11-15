#include "driver/counter.h"

Counter counter;

ISR(TIMER1_OVF_vect)
{
	if (counter.overflowed < 255) {
		counter.overflowed++;
	}
}
