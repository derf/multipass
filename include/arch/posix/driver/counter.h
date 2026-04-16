/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>
#include <time.h>

#ifdef CONFIG_arch_posix_driver_counter_tsc
#include <x86intrin.h>
#endif

typedef uint64_t counter_value_t;
typedef uint8_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);
#ifdef CONFIG_arch_posix_driver_counter_tsc
		uint64_t start_tsc;
#else
		int64_t start_sec, start_nsec;
#endif

	public:
		uint64_t value;
		volatile uint8_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
#ifdef CONFIG_arch_posix_driver_counter_tsc
			start_tsc = __rdtsc();
#else
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			start_sec = ts.tv_sec;
			start_nsec = ts.tv_nsec;
#endif
		}

		inline void stop() {
#ifdef CONFIG_arch_posix_driver_counter_tsc
			uint64_t stop_tsc = __rdtsc();
			value = stop_tsc - start_tsc;
#else
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);

			value = (ts.tv_sec - start_sec) * 1000000000L;
			value += ts.tv_nsec - start_nsec;
#endif
		}
};

extern Counter counter;

#endif
