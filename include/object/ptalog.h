#ifndef PTALOG_H
#define PTALOG_H

#include <stdlib.h>
#include "driver/stdout.h"

class PTALog {
	public:
		typedef struct {
			uint8_t transition_id;
		} log_entry;

		log_entry log[32];
		uint8_t log_index;

		PTALog(uint8_t pin_number) : log_index(0), sync_pin(pin_number) {}

		inline void passTransition(uint8_t transition_id)
		{
			log[log_index].transition_id = transition_id;
			log_index++;
		}

		inline void reset()
		{
			log_index = 0;
		}

		inline void startBenchmark(uint8_t id)
		{
			kout << "[PTA] benchmark start, id=" << dec << id << endl;
			gpio.output(sync_pin);
		}

		inline void stopBenchmark()
		{
			kout << "[PTA] benchmark stop" << endl;
		}

		void dump()
		{
			kout << "[PTA] trace, count=" << dec << log_index << endl;
			for (uint8_t i = 0; i < log_index; i++) {
				kout << "[PTA] transition=" << log[i].transition_id << endl;
			}
		}

		inline void startTransition()
		{
			gpio.write(sync_pin, 1);
		}

		inline void stopTransition()
		{
			gpio.write(sync_pin, 0);
		}

	private:
		PTALog(const PTALog& copy);
		uint8_t sync_pin;
};

#endif
