/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/counter.h"

#if defined(TIMER_S) || defined(WITH_LOOP)
#warn "timer/loop and counter are mutually exclusive. Expect odd behaviour."
#endif

Counter counter;

ISR(TIMER1_OVF_vect)
{
	if (counter.overflow < 255) {
		counter.overflow++;
	}
}
