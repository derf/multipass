#include <msp430.h>
#include <stdint.h>

#define ON_TIMER_INTERRUPT_head __attribute__((interrupt(TIMER0_A1_VECTOR))) __attribute__((wakeup)) void handle_timer0_overflow() { if (TA0IV == 0x0e) {
#define ON_TIMER_INTERRUPT_tail } }

#if F_CPU == 16000000UL
#define _TA0_MAIN_DIV ID__8
#elif F_CPU == 8000000UL
#define _TA0_MAIN_DIV ID__4
#elif F_CPU == 4000000UL
#define _TA0_MAIN_DIV ID__2
#elif F_CPU == 1000000UL
#define _TA0_MAIN_DIV ID__1
#else
#error Unsupported F_CPU
#endif

class Timer {
	private:
		Timer(const Timer &copy);

	public:
		Timer() {}

#if F_CPU == 1000000UL
		inline void setup_khz(uint16_t const frequency) {
			TA0CTL = TASSEL__SMCLK | ID__1 | MC__UP; // -> 1 MHz base
			TA0EX0 = 0;
			TA0CCR0 = 1000UL / frequency;
			TA0CTL |= TACLR;
		}

		inline void setup_hz(uint16_t const frequency) { // 1 MHz base
			if (frequency < 20) {
				TA0CTL = TASSEL__SMCLK | ID__8 | MC__UP; // /8
				TA0EX0 = 1; // /2 -> /16 -> 62500 Hz
				TA0CCR0 = 62500UL / frequency;
			} else {
				TA0CTL = TASSEL__SMCLK | ID__1 | MC__UP;
				TA0EX0 = 0;
				TA0CCR0 = 1000000UL / frequency;
			}
			TA0CTL |= TACLR;
		}
#else
		inline void setup_khz(uint16_t const frequency) {
			TA0CTL = TASSEL__SMCLK | _TA0_MAIN_DIV | MC__UP; // -> 2 MHz base
			TA0EX0 = 0;
			TA0CCR0 = 2000UL / frequency;
			TA0CTL |= TACLR;
		}

		inline void setup_hz(uint16_t const frequency) { // 2 MHz base
			TA0CTL = TASSEL__SMCLK | _TA0_MAIN_DIV | MC__UP;
			TA0EX0 = 0;
			TA0CCR0 = 2000000UL / frequency;
			TA0CTL |= TACLR;
		}
#endif

		inline void start(unsigned char const interrupt) {
			if (interrupt) {
				TA0CTL |= TACLR | TAIE;
			} else {
				TA0CTL |= TACLR;
			}
		}

		inline void stop() {
			TA0CTL &= ~TAIE;
		}
};

extern Timer timer;
