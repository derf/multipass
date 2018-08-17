#include <avr/io.h>
#include <avr/interrupt.h>

#define ON_TIMER_INTERRUPT ISR(TIMER0_COMPA_vect)

class Timer {
	private:
		Timer(const Timer &copy);


	public:
		Timer() {}

		inline void setup(unsigned char const frequency) {
			OCR0A = frequency ? 255 / frequency : 1;
			TCCR0A = _BV(WGM01);
		}
		inline void start(unsigned char const interrupt) {
			TCNT0 = 0;
			TCCR0B = _BV(CS01) | _BV(CS00);
			if (interrupt) {
				TIMSK0 = _BV(OCIE0A);
			}
		}
		inline void stop() { TCCR0B = 0; TIMSK0 = 0; }
};

extern Timer timer;
