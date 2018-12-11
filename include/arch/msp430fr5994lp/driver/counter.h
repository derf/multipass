#include <msp430.h>
#include <stdint.h>

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint16_t value;
		uint8_t overflowed;

		Counter() : overflowed(0) {}

		inline void start() {
			overflowed = 0;
			TA2CTL = TASSEL__SMCLK | ID__1 | MC__CONTINUOUS;
			TA2EX0 = 0;
			TA2CTL |= TACLR;
		}

		inline void stop() {
			TA2CTL = 0;
			value = TA2R;
		}
};

extern Counter counter;
