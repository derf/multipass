#ifndef COUNTER_H
#define COUNTER_H

extern "C" {
#include "osapi.h"
#include "user_interface.h"
}
#include "c_types.h"

class Counter {
	private:
		Counter(const Counter &copy);
		uint32_t start_cycles;

	public:
		Counter() : start_cycles(0) {}
		uint32_t overflowed;

		inline void start() {
			asm volatile ("esync; rsr %0,ccount":"=a" (start_cycles));
		}

		inline uint32_t stop() {
			uint32_t stop_cycles;
			asm volatile ("esync; rsr %0,ccount":"=a" (stop_cycles));
			if (stop_cycles > start_cycles) {
				return stop_cycles - start_cycles;
			} else {
				return 0;
			}
		}
};

extern Counter counter;

#endif
