/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#define STM_TIM0 (*(volatile unsigned int*)0xf0000210)
#define STM_CAP (*(volatile unsigned int*)0xf000022c)

typedef unsigned int counter_value_t;
typedef unsigned int counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);
		unsigned long long startvalue, stopvalue;

	public:
		counter_value_t value;
		counter_overflow_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			startvalue = STM_TIM0;
			startvalue += (unsigned long long)STM_CAP << 32;
		}

		inline void stop() {
			stopvalue = STM_TIM0;
			stopvalue += (unsigned long long)STM_CAP << 32;
			value = (stopvalue - startvalue) & 0xffffffff;
			overflow = (unsigned long long)(stopvalue - startvalue) >> 32;
		}
};

extern Counter counter;

#endif
