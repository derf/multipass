/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/counter.h"

Counter counter;

#ifndef __acweaving
__attribute__((interrupt(TIMER2_A1_VECTOR))) void handle_timer2_overflow()
{
	if (TA2IV == 0x0e) {
		if (counter.overflow < 65535) {
			counter.overflow++;
		}
	}
}
#endif
