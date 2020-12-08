/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/counter.h"

Counter counter;

void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_UIF)) {
		timer_clear_flag(TIM2, TIM_SR_UIF);
		if (counter.overflow < 4294967295) {
			counter.overflow++;
		}
	}
}
