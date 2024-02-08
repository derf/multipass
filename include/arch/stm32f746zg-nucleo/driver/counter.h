/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <libopencm3/stm32/timer.h>

#include "arch.h"

typedef uint32_t counter_value_t;
typedef uint32_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		counter_value_t value;
		volatile counter_overflow_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			overflow = 0;
			timer_set_counter(TIM2, 0);
			timer_enable_counter(TIM2);
		}

		inline void stop() {
			timer_disable_counter(TIM2);
			value = timer_get_counter(TIM2);
		}
};

extern Counter counter;

#endif
