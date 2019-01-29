#include <stdint.h>
#include <time.h>

class Counter {
	private:
		Counter(const Counter &copy);
		uint64_t start_sec, start_nsec;

	public:
		uint64_t value;
		volatile uint8_t overflowed;

		Counter() : overflowed(0) {}

		inline void start() {
			struct timespec ts;
			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
			start_sec = ts.tv_sec;
			start_nsec = ts.tv_nsec;
		}

		inline void stop() {
			struct timespec ts;
			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);

			value = (ts.tv_sec - start_sec) * 1000000000UL;
			value += ts.tv_nsec - start_nsec;
		}
};

extern Counter counter;