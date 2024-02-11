/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <msp430.h>
#include <stdint.h>

typedef uint16_t counter_value_t;
typedef uint16_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint16_t value;
		volatile uint16_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			overflow = 0;
			TA2CTL = TASSEL__SMCLK | ID__1 | MC__CONTINUOUS;
			TA2EX0 = 0;
			TA2CTL |= TACLR;
			asm volatile("nop");
			TA2CTL |= TAIE;
			asm volatile("nop");
			__eint();
			asm volatile("nop");
		}

		inline void stop() {
			TA2CTL = 0;
			value = TA2R;
			__dint();
		}
};

extern Counter counter;

#endif
