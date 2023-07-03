/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint16_t counter_value_t;
typedef uint8_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint16_t value;
		volatile uint8_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			overflow = 0;
			TCNT3 = 0;
			TCCR3A = 0;
			TCCR3B = _BV(CS10); // no prescaler
			TIMSK3 = _BV(TOIE3);
		}

		inline void stop() {
			TCCR3B = 0;
			value = TCNT3;
		}
};

extern Counter counter;

#endif
