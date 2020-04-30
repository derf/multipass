#ifndef GPIO_H
#define GPIO_H

#include <msp430.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			p1_0 = 0, p1_1, p1_2, p1_3, p1_4, p1_5, p1_6, p1_7,
			p2_0, p2_1, p2_2, p2_3, p2_4, p2_5, p2_6, p2_7,
			p3_0, p3_1, p3_2, p3_3, p3_4, p3_5, p3_6, p3_7,
			p4_0, p4_1, p4_2, p4_3, p4_4, p4_5, p4_6, p4_7,
			pj_0, pj_1, pj_2, pj_3, pj_4, pj_5, pj_6, pj_7,
			PIN_INVALID
		};

		/*
		 * Set all non-standard GPIOs to input with pull-down.
		 * This avoids excessive current draw due to floating inputs.
		 * Note that we do not set GPIOs to output low, as that might
		 * short-circuit other peripherals.
		 */
		inline void setup() {
			PIDIR = BIT0; // green led
			P2DIR = 0;
			P3DIR = 0;
			P4DIR = BIT6; // red LED
			PJDIR = 0;
			P1OUT = 0;
			P2OUT = 0;
			P3OUT = 0;
			P4OUT = 0;
			PJOUT = 0;
			P1REN = 0xff & ~BIT0; // green LED
			P2REN = 0xff & ~(BIT0 | BIT1); // UART
			P3REN = 0xff;
			P4REN = 0xff & ~BIT6; // red LED
			PJREN = BIT6 | BIT7; // HFXT (not populated)
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
		inline void input(unsigned char const pin) {
			if (pin < p2_0) {
				P1DIR &= ~(1 << pin);
			} else if (pin < p3_0) {
				P2DIR &= ~(1 << (pin - p2_0));
			} else if (pin < p4_0) {
				P3DIR &= ~(1 << (pin - p3_0));
			} else if (pin < pj_0) {
				P4DIR &= ~(1 << (pin - p4_0));
			} else if (pin < PIN_INVALID) {
				PJDIR &= ~(1 << (pin - pj_0));
			}
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
			if (pin < p2_0) {
				P1DIR &= ~(1 << pin);
				P1OUT = pull ? (P1OUT | (1 << pin)) : (P1OUT & ~(1 << pin));
				P1REN |= (1 << pin);
			} else if (pin < p3_0) {
				P2DIR &= ~(1 << (pin - p2_0));
				P2OUT = pull ? (P2OUT | (1 << (pin - p2_0))) : (P2OUT & ~(1 << (pin - p2_0)));
				P2REN |= (1 << (pin - p2_0));
			} else if (pin < p4_0) {
				P3DIR &= ~(1 << (pin - p3_0));
				P3OUT = pull ? (P3OUT | (1 << (pin - p3_0))) : (P3OUT & ~(1 << (pin - p3_0)));
				P3REN |= (1 << (pin - p3_0));
			} else if (pin < pj_0) {
				P4DIR &= ~(1 << (pin - p4_0));
				P4OUT = pull ? (P4OUT | (1 << (pin - p4_0))) : (P4OUT & ~(1 << (pin - p4_0)));
				P4REN |= (1 << (pin - p4_0));
			} else if (pin < PIN_INVALID) {
				PJDIR &= ~(1 << (pin - pj_0));
				PJOUT = pull ? (PJOUT | (1 << (pin - pj_0))) : (PJOUT & ~(1 << (pin - pj_0)));
				PJREN |= (1 << (pin - pj_0));
			}
		}
		inline void output(unsigned char const pin) {
			if (pin < p2_0) {
				P1DIR |= (1 << pin);
			} else if (pin < p3_0) {
				P2DIR |= (1 << (pin - p2_0));
			} else if (pin < p4_0) {
				P3DIR |= (1 << (pin - p3_0));
			} else if (pin < pj_0) {
				P4DIR |= (1 << (pin - p4_0));
			} else if (pin < PIN_INVALID) {
				PJDIR |= (1 << (pin - pj_0));
			}
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			if (pin < p2_0) {
				P1OUT = value ? (P1OUT | (1 << pin)) : (P1OUT & ~(1 << pin));
				P1DIR |= (1 << pin);
			} else if (pin < p3_0) {
				P2OUT = value ? (P2OUT | (1 << (pin - p2_0))) : (P2OUT & ~(1 << (pin - p2_0)));
				P2DIR |= (1 << (pin - p2_0));
			} else if (pin < p4_0) {
				P3OUT = value ? (P3OUT | (1 << (pin - p3_0))) : (P3OUT & ~(1 << (pin - p3_0)));
				P3DIR |= (1 << (pin - p3_0));
			} else if (pin < pj_0) {
				P4OUT = value ? (P4OUT | (1 << (pin - p4_0))) : (P4OUT & ~(1 << (pin - p4_0)));
				P4DIR |= (1 << (pin - p4_0));
			} else if (pin < PIN_INVALID) {
				PJOUT = value ? (PJOUT | (1 << (pin - pj_0))) : (PJOUT & ~(1 << (pin - pj_0)));
				PJDIR |= (1 << (pin - pj_0));
			}
		}
		inline unsigned char read(unsigned char const pin) {
			if (pin < p2_0) {
				return P1IN & (1 << pin);
			} else if (pin < p3_0) {
				return P2IN & (1 << (pin - p2_0));
			} else if (pin < p4_0) {
				return P3IN & (1 << (pin - p3_0));
			} else if (pin < pj_0) {
				return P4IN & (1 << (pin - p4_0));
			} else if (pin < PIN_INVALID) {
				return PJIN & (1 << (pin - pj_0));
			}
			return 0;
		}
		inline void write(unsigned char const pin, unsigned char value) {
			if (pin < p2_0) {
				if (value) {
					P1OUT |= (1 << pin);
				} else {
					P1OUT &= ~(1 << pin);
				}
			} else if (pin < p3_0) {
				if (value) {
					P2OUT |= (1 << (pin - p2_0));
				} else {
					P2OUT &= ~(1 << (pin - p2_0));
				}
			} else if (pin < p4_0) {
				if (value) {
					P3OUT |= (1 << (pin - p3_0));
				} else {
					P3OUT &= ~(1 << (pin - p3_0));
				}
			} else if (pin < pj_0) {
				if (value) {
					P4OUT |= (1 << (pin - p4_0));
				} else {
					P4OUT &= ~(1 << (pin - p4_0));
				}
			} else if (pin < PIN_INVALID) {
				if (value) {
					PJOUT |= (1 << (pin - pj_0));
				} else {
					PJOUT &= ~(1 << (pin - pj_0));
				}
			}
		}
		inline void write_mask(unsigned char const pin_base, unsigned char set_mask, unsigned char clear_mask) {
			if (pin_base < p2_0) {
				P1OUT = (P1OUT | set_mask) & ~clear_mask;
			}
			if (pin_base < p3_0) {
				P2OUT = (P2OUT | set_mask) & ~clear_mask;
			}
			if (pin_base < p4_0) {
				P3OUT = (P3OUT | set_mask) & ~clear_mask;
			}
			if (pin_base < pj_0) {
				P4OUT = (P4OUT | set_mask) & ~clear_mask;
			}
			if (pin_base < PIN_INVALID) {
				PJOUT = (PJOUT | set_mask) & ~clear_mask;
			}
		}
};

extern GPIO gpio;

#endif
