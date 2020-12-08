/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

extern "C" {
#include "osapi.h"
#include "user_interface.h"
}
#include "c_types.h"

typedef uint32_t counter_value_t;
typedef uint32_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);
		uint32_t start_cycles;

	public:
		uint32_t value;
		uint32_t overflow;

		Counter() : start_cycles(0), value(0), overflow(0) {}

		inline void start() {
			asm volatile ("esync; rsr %0,ccount":"=a" (start_cycles));
		}

		inline void stop() {
			uint32_t stop_cycles;
			asm volatile ("esync; rsr %0,ccount":"=a" (stop_cycles));
			if (stop_cycles > start_cycles) {
				value = stop_cycles - start_cycles;
			} else {
				overflow = 1;
			}
		}
};

extern Counter counter;

#endif
