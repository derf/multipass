/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/counter.h"

Counter counter;

ISR(TIMER3_OVF_vect)
{
	if (counter.overflow < 255) {
		counter.overflow++;
	}
}
