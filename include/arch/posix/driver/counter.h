/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>
#include <time.h>

typedef int64_t counter_value_t;
typedef uint8_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);
		int64_t start_sec, start_nsec;

	public:
		uint64_t value;
		volatile uint8_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			start_sec = ts.tv_sec;
			start_nsec = ts.tv_nsec;
		}

		inline void stop() {
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);

			value = (ts.tv_sec - start_sec) * 1000000000L;
			value += ts.tv_nsec - start_nsec;
		}
};

extern Counter counter;

#endif
