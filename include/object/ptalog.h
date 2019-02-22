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

		PTALog() : log_index(0) {}

		void passTransition(uint8_t transition_id)
		{
			log[log_index].transition_id = transition_id;
			log_index++;
		}

		void reset()
		{
			log_index = 0;
		}

		void startBenchmark(uint8_t id)
		{
			kout << "[PTA] benchmark start, id=" << dec << id << endl;
		}

		void stopBenchmark()
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

	private:
		PTALog(const PTALog& copy);
};

#endif
