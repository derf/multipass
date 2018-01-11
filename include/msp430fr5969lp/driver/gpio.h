#ifndef GPIO_H
#define GPIO_H

#include <msp430.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}
		inline void setup() {
			P1OUT = 0;
			P2OUT = 0;
			P3OUT = 0;
			P4OUT = 0;
			P1DIR = BIT0;
			P2DIR = 0;
			P3DIR = 0;
			P4DIR = BIT6;
		}
		inline void led_on(unsigned char id) {
			if (id == 0) {
				P1OUT |= BIT0;
			} else {
				P4OUT |= BIT6;
			}
		}
		inline void led_off(unsigned char id) {
			if (id == 0) {
				P1OUT &= ~BIT0;
			} else {
				P4OUT &= ~BIT6;
			}
		}
		inline void led_toggle(unsigned char id) {
			if (id == 0) {
				P1OUT ^= BIT0;
			} else {
				P4OUT ^= BIT6;
			}
		}
};

extern GPIO gpio;

#endif
