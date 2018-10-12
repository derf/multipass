#include <msp430.h>

#define ON_TIMER_INTERRUPT_head __attribute__((interrupt(TIMER0_A1_VECTOR))) __attribute__((wakeup)) void handle_timer0_overflow() { if (TA0IV == 0x0e) {
#define ON_TIMER_INTERRUPT_tail } }

class Timer {
	private:
		Timer(const Timer &copy);

	public:
		Timer() {}

		inline void setup_khz(uint16_t const frequency) {
			TA0CTL = TASSEL__SMCLK | ID__8 | MC__UP;
			TA0EX0 = 1;
			TA0CCR0 = 1000 / frequency;
			TA0CTL |= TACLR;
		}

		inline void setup_hz(uint16_t const frequency) {
			TA0CTL = TASSEL__SMCLK | ID__8 | MC__UP;
			TA0EX0 = 1;
			TA0CCR0 = 1000000 / frequency;
			TA0CTL |= TACLR;
		}

		inline void start(unsigned char const interrupt) {
			if (interrupt) {
				TA0CTL |= TACLR | TAIE;
			} else {
				TA0CTL |= TACLR;
			}
		}

		inline void stop() {
			TA0CTL = 0;
		}
};

extern Timer timer;
