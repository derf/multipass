#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint16_t counter_value_t;
typedef uint8_t counter_overflow_t;

class Counter {
	private:
		Counter(const Counter &copy);

	public:
		uint16_t value;
		volatile uint8_t overflow;

		Counter() : overflow(0) {}

		inline void start() {
			overflow = 0;
			TCNT1 = 0;
			TCCR1A = 0;
			TCCR1B = _BV(CS10); // no prescaler
			TIMSK1 = _BV(TOIE1);
		}

		inline void stop() {
			TCCR1B = 0;
			value = TCNT1;
		}
};

extern Counter counter;
