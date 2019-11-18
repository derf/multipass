#ifndef PTALOG_H
#define PTALOG_H

#include <stdlib.h>
#include "driver/stdout.h"

#ifdef PTALOG_GPIO
#include "driver/gpio.h"
#if defined(PTALOG_GPIO_BEFORE) || defined(PTALOG_GPIO_BAR)
#include "arch.h"
#endif
#endif

#ifdef PTALOG_TIMING
#include "driver/counter.h"
#endif

class PTALog {

	private:
		PTALog(const PTALog& copy);
#ifdef PTALOG_GPIO
		uint8_t sync_pin;
#endif

	public:
		typedef struct {
			uint8_t transition_id;
#ifdef PTALOG_TIMING
			counter_value_t timer;
			counter_overflow_t overflow;
#endif
#ifdef PTALOG_WITH_RETURNVALUES
			uint16_t return_value;
#endif
		} log_entry;

		int const max_entry = 15;

		log_entry log[16];
		uint8_t log_index;

#ifdef PTALOG_GPIO
		PTALog(uint8_t pin_number) : sync_pin(pin_number), log_index(0) {}
#else
		PTALog() : log_index(0) {}
#endif

		void passTransition(uint8_t transition_id)
		{
			log[log_index].transition_id = transition_id;
			if (log_index < max_entry) {
				log_index++;
			}
		}

#ifdef PTALOG_TIMING
		void passNop(Counter& counter)
		{
			kout << "[PTA] nop=" << counter.value << "/" << counter.overflow << endl;
		}
#endif

		void reset()
		{
			log_index = 0;
		}

		void startBenchmark(uint8_t id)
		{
			kout << "[PTA] benchmark start, id=" << dec << id << endl;
#ifdef PTALOG_GPIO
			gpio.output(sync_pin);
#endif
		}

		void stopBenchmark()
		{
			kout << "[PTA] benchmark stop" << endl;
		}

		void dump(uint16_t trace_id)
		{
			kout << "[PTA] trace=" << dec << trace_id << " count=" << log_index << endl;
			for (uint8_t i = 0; i < log_index; i++) {
				kout << "[PTA] transition=" << log[i].transition_id;
#ifdef PTALOG_TIMING
				kout << " cycles=" << log[i].timer << "/" << log[i].overflow;
#endif
#ifdef PTALOG_WITH_RETURNVALUES
				kout << " return=" << log[i].return_value;
#endif
				kout << endl;
			}
		}

#ifdef PTALOG_GPIO_BAR
		void startTransition(char const *code, uint8_t code_length)
		{
			// Quiet zone (must last at least 10x longer than a module)
			arch.sleep_ms(60);
			for (uint8_t byte = 0; byte < code_length; byte++) {
				for (uint16_t mask = 0x01; mask <= 0x80; mask <<= 1) {
					// a single module, which is part of a bar
					gpio.write(sync_pin, code[byte] & mask ? 1 : 0);
					arch.sleep_ms(5);
				}
			}
			gpio.write(sync_pin, 0);
			// Quiet zone
			arch.sleep_ms(60);
		}
#else
		inline void startTransition()
		{
#ifdef PTALOG_GPIO_BEFORE
			gpio.write(sync_pin, 1);
			arch.sleep_ms(10);
			gpio.write(sync_pin, 0);
			arch.sleep_ms(10);
#else
			gpio.write(sync_pin, 1);
#endif
		}
#endif

#ifdef PTALOG_WITH_RETURNVALUES
		inline void logReturn(uint16_t ret)
		{
			log[log_index - 1].return_value = ret;
		}
#endif

#ifdef PTALOG_TIMING
		inline void stopTransition(Counter& counter)
#else
		inline void stopTransition()
#endif
		{
#ifdef PTALOG_GPIO
#if !defined(PTALOG_GPIO_BEFORE) && !defined(PTALOG_GPIO_BAR)
			gpio.write(sync_pin, 0);
#endif
#endif
#ifdef PTALOG_TIMING
			log[log_index - 1].timer = counter.value;
			log[log_index - 1].overflow = counter.overflow;
#endif
		}
};

#endif
