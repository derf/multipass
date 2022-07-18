/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef COUNTER_H
#define COUNTER_H

#include "rti.h"
#include <stdint.h>

typedef uint32_t counter_value_t;
typedef uint32_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint32_t value;
		uint32_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			rtiREG1->CNT[0].UCx = 0;
			rtiREG1->CNT[0].FRCx = 0;
			rtiREG1->GCTRL |= ((uint32)1 << (rtiCOUNTER_BLOCK0 & 3));
		}

		inline void stop() {
			rtiREG1->GCTRL &= ~(uint32)((uint32)1 << (rtiCOUNTER_BLOCK0 & 3));
			overflow = rtiREG1->CNT[0].FRCx;
			value = rtiREG1->CNT[0].UCx;
		}
};

extern Counter counter;

#endif
