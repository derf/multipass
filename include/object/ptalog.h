#ifndef PTALOG_H
#define PTALOG_H

#include <stdlib.h>
#include "driver/stdout.h"

#ifdef PTALOG_GPIO
#include "driver/gpio.h"
#endif

#ifdef PTALOG_TIMING
#include "driver/counter.h"
#endif

class PTALog {
	public:
		typedef struct {
			uint8_t transition_id;
#ifdef PTALOG_TIMING
			counter_value_t timer;
			counter_overflow_t overflow;
#endif
		} log_entry;

		int const max_entry = 15;

		log_entry log[16];
		uint8_t log_index;

#ifdef PTALOG_GPIO
		PTALog(uint8_t pin_number) : log_index(0), sync_pin(pin_number) {}
#else
		PTALog() : log_index(0) {}
#endif

		inline void passTransition(uint8_t transition_id)
		{
			log[log_index].transition_id = transition_id;
			if (log_index < max_entry) {
				log_index++;
			}
		}

#ifdef PTALOG_TIMING
		inline void passNop(Counter& counter)
		{
			kout << "[PTA] nop=" << counter.value << "/" << counter.overflow << endl;
		}
#endif

		inline void reset()
		{
			log_index = 0;
		}

		inline void startBenchmark(uint8_t id)
		{
			kout << "[PTA] benchmark start, id=" << dec << id << endl;
#ifdef PTALOG_GPIO
			gpio.output(sync_pin);
#endif
		}

		inline void stopBenchmark()
		{
			kout << "[PTA] benchmark stop" << endl;
		}

		void dump(uint16_t trace_id)
		{
			kout << "[PTA] trace=" << dec << trace_id << ", count=" << log_index << endl;
			for (uint8_t i = 0; i < log_index; i++) {
#ifdef PTALOG_TIMING
				kout << "[PTA] transition=" << log[i].transition_id;
				kout << " cycles=" << log[i].timer << "/" << log[i].overflow << endl;
#else
				kout << "[PTA] transition=" << log[i].transition_id << endl;
#endif
			}
		}

		inline void startTransition()
		{
			gpio.write(sync_pin, 1);
		}

#ifdef PTALOG_TIMING
		inline void stopTransition(Counter& counter)
#else
		inline void stopTransition()
#endif
		{
#ifdef PTALOG_GPIO
			gpio.write(sync_pin, 0);
#endif
#ifdef PTALOG_TIMING
			log[log_index - 1].timer = counter.value;
			log[log_index - 1].overflow = counter.overflow;
#endif
		}

	private:
		PTALog(const PTALog& copy);
#ifdef PTALOG_GPIO
		uint8_t sync_pin;
#endif
};

#endif
