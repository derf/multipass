#include <msp430.h>
#include <stdint.h>

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint8_t overflowed;

		Counter() : overflowed(0) {}

		inline void start() {
			overflowed = 0;
			TA2CTL = TASSEL__SMCLK | ID__1 | MC__CONTINUOUS;
			TA2EX0 = 0;
			TA2CTL |= TACLR;
		}

		inline uint16_t stop() {
			TA2CTL = 0;
			return TA2R;
		}
};

extern Counter counter;
