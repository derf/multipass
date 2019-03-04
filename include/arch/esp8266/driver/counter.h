#ifndef COUNTER_H
#define COUNTER_H

extern "C" {
#include "osapi.h"
#include "user_interface.h"
}
#include "c_types.h"

typedef counter_value_t uint32_t;
typedef counter_overflowed_t uint32_t;

class Counter {
	private:
		Counter(const Counter &copy);
		uint32_t start_cycles;

	public:
		uint32_t value;
		uint32_t overflowed;

		Counter() : start_cycles(0), value(0), overflowed(0) {}

		inline void start() {
			asm volatile ("esync; rsr %0,ccount":"=a" (start_cycles));
		}

		inline void stop() {
			uint32_t stop_cycles;
			asm volatile ("esync; rsr %0,ccount":"=a" (stop_cycles));
			if (stop_cycles > start_cycles) {
				value = stop_cycles - start_cycles;
			} else {
				overflowed = 1;
			}
		}
};

extern Counter counter;

#endif
