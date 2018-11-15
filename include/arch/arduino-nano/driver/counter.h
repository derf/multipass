#include <avr/io.h>
#include <avr/interrupt.h>

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint8_t overflowed;

		Counter() : overflowed(0) {}

		inline void start() {
			overflowed = 0;
			TCNT1 = 0;
			TCCR1A = 0;
			TCCR1B = _BV(CS10);
			TIMSK1 = _BV(TOIE1);
		}

		inline uint16_t stop() {
			TCCR1B = 0;
			return TCNT1;
		}
};

extern Counter counter;
