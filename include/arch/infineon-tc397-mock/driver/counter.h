/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

typedef unsigned int counter_value_t;
typedef unsigned int counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		counter_value_t value;
		volatile counter_overflow_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
		}

		inline void stop() {
		}
};

extern Counter counter;

#endif
