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
			p5_0, p5_1, p5_2, p5_3, p5_4, p5_5, p5_6, p5_7,
			p6_0, p6_1, p6_2, p6_3, p6_4, p6_5, p6_6, p6_7,
			p7_0, p7_1, p7_2, p7_3, p7_4, p7_5, p7_6, p7_7,
			p8_0, p8_1, p8_2, p8_3, p8_4, p8_5, p8_6, p8_7,
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
			P1DIR = BIT0 | BIT1; // red LED, green LED
			P2DIR = 0;
			P3DIR = 0;
			P4DIR = 0;
			P5DIR = 0;
			P6DIR = 0;
			P7DIR = 0;
			P8DIR = 0;
			PJDIR = 0;
			P1OUT = 0;
			P2OUT = 0;
			P3OUT = 0;
			P4OUT = 0;
			P5OUT = 0;
			P6OUT = 0;
			P7OUT = 0;
			P8OUT = 0;
			PJOUT = 0;
			P1REN = 0xff & ~(BIT0 | BIT1); // red LED, green LED
			P2REN = 0xff & ~(BIT0 | BIT1); // UART
			P3REN = 0xff;
			P4REN = 0xff;
			P5REN = 0xff;
			P6REN = 0xff;
			P7REN = 0xff;
			P8REN = 0xff;
			PJREN = BIT6 | BIT7; // HFXT (not populated)
		}
		inline void led_on(unsigned char id) {
			if (id == 0) {
				P1OUT |= BIT0;
			} else {
				P1OUT |= BIT1;
			}
		}
		inline void led_off(unsigned char id) {
			if (id == 0) {
				P1OUT &= ~BIT0;
			} else {
				P1OUT &= ~BIT1;
			}
		}
		inline void led_toggle(unsigned char id) {
			if (id == 0) {
				P1OUT ^= BIT0;
			} else {
				P1OUT ^= BIT1;
			}
		}
		inline void input(unsigned char const pin) {
			if (pin < p2_0) {
				P1DIR &= ~(1 << pin);
			} else if (pin < p3_0) {
				P2DIR &= ~(1 << (pin - p2_0));
			} else if (pin < p4_0) {
				P3DIR &= ~(1 << (pin - p3_0));
			} else if (pin < p5_0) {
				P4DIR &= ~(1 << (pin - p4_0));
			} else if (pin < p6_0) {
				P5DIR &= ~(1 << (pin - p5_0));
			} else if (pin < p7_0) {
				P6DIR &= ~(1 << (pin - p6_0));
			} else if (pin < p8_0) {
				P7DIR &= ~(1 << (pin - p7_0));
			} else if (pin < pj_0) {
				P8DIR &= ~(1 << (pin - p8_0));
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
			} else if (pin < p5_0) {
				P4DIR &= ~(1 << (pin - p4_0));
				P4OUT = pull ? (P4OUT | (1 << (pin - p4_0))) : (P4OUT & ~(1 << (pin - p4_0)));
				P4REN |= (1 << (pin - p4_0));
			} else if (pin < p6_0) {
				P5DIR &= ~(1 << (pin - pj_0));
				P5OUT = pull ? (P5OUT | (1 << (pin - p5_0))) : (P5OUT & ~(1 << (pin - p5_0)));
				P5REN |= (1 << (pin - pj_0));
			} else if (pin < p7_0) {
				P6DIR &= ~(1 << (pin - p2_0));
				P6OUT = pull ? (P6OUT | (1 << (pin - p6_0))) : (P6OUT & ~(1 << (pin - p6_0)));
				P6REN |= (1 << (pin - p2_0));
			} else if (pin < p8_0) {
				P7DIR &= ~(1 << (pin - p3_0));
				P7OUT = pull ? (P7OUT | (1 << (pin - p7_0))) : (P7OUT & ~(1 << (pin - p7_0)));
				P7REN |= (1 << (pin - p3_0));
			} else if (pin < pj_0) {
				P8DIR &= ~(1 << (pin - p4_0));
				P8OUT = pull ? (P8OUT | (1 << (pin - p8_0))) : (P8OUT & ~(1 << (pin - p8_0)));
				P8REN |= (1 << (pin - p4_0));
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
			} else if (pin < p5_0) {
				P4DIR |= (1 << (pin - p4_0));
			} else if (pin < p6_0) {
				P5DIR |= (1 << (pin - p5_0));
			} else if (pin < p7_0) {
				P6DIR |= (1 << (pin - p6_0));
			} else if (pin < p8_0) {
				P7DIR |= (1 << (pin - p7_0));
			} else if (pin < pj_0) {
				P8DIR |= (1 << (pin - p8_0));
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
			} else if (pin < p5_0) {
				P4OUT = value ? (P4OUT | (1 << (pin - p4_0))) : (P4OUT & ~(1 << (pin - p4_0)));
				P4DIR |= (1 << (pin - p4_0));
			} else if (pin < p6_0) {
				P5OUT = value ? (P5OUT | (1 << (pin - p5_0))) : (P5OUT & ~(1 << (pin - p5_0)));
				P5DIR |= (1 << (pin - p5_0));
			} else if (pin < p7_0) {
				P6OUT = value ? (P6OUT | (1 << (pin - p6_0))) : (P6OUT & ~(1 << (pin - p6_0)));
				P6DIR |= (1 << (pin - p6_0));
			} else if (pin < p8_0) {
				P7OUT = value ? (P7OUT | (1 << (pin - p7_0))) : (P7OUT & ~(1 << (pin - p7_0)));
				P7DIR |= (1 << (pin - p7_0));
			} else if (pin < pj_0) {
				P8OUT = value ? (P8OUT | (1 << (pin - p8_0))) : (P8OUT & ~(1 << (pin - p8_0)));
				P8DIR |= (1 << (pin - p8_0));
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
			} else if (pin < p5_0) {
				return P4IN & (1 << (pin - p4_0));
			} else if (pin < p6_0) {
				return P5IN & (1 << (pin - p5_0));
			} else if (pin < p7_0) {
				return P6IN & (1 << (pin - p6_0));
			} else if (pin < p8_0) {
				return P7IN & (1 << (pin - p7_0));
			} else if (pin < pj_0) {
				return P8IN & (1 << (pin - p8_0));
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
			} else if (pin < p5_0) {
				if (value) {
					P4OUT |= (1 << (pin - p4_0));
				} else {
					P4OUT &= ~(1 << (pin - p4_0));
				}
			} else if (pin < p6_0) {
				if (value) {
					P5OUT |= (1 << (pin - p5_0));
				} else {
					P5OUT &= ~(1 << (pin - p5_0));
				}
			} else if (pin < p7_0) {
				if (value) {
					P6OUT |= (1 << (pin - p6_0));
				} else {
					P6OUT &= ~(1 << (pin - p6_0));
				}
			} else if (pin < p8_0) {
				if (value) {
					P7OUT |= (1 << (pin - p7_0));
				} else {
					P7OUT &= ~(1 << (pin - p7_0));
				}
			} else if (pin < pj_0) {
				if (value) {
					P8OUT |= (1 << (pin - p8_0));
				} else {
					P8OUT &= ~(1 << (pin - p8_0));
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
			} else if (pin_base < p3_0) {
				P2OUT = (P2OUT | set_mask) & ~clear_mask;
			} else if (pin_base < p4_0) {
				P3OUT = (P3OUT | set_mask) & ~clear_mask;
			} else if (pin_base < p5_0) {
				P4OUT = (P4OUT | set_mask) & ~clear_mask;
			} else if (pin_base < p6_0) {
				P5OUT = (P5OUT | set_mask) & ~clear_mask;
			} else if (pin_base < p7_0) {
				P6OUT = (P6OUT | set_mask) & ~clear_mask;
			} else if (pin_base < p8_0) {
				P7OUT = (P7OUT | set_mask) & ~clear_mask;
			} else if (pin_base < pj_0) {
				P8OUT = (P8OUT | set_mask) & ~clear_mask;
			} else if (pin_base < PIN_INVALID) {
				PJOUT = (PJOUT | set_mask) & ~clear_mask;
			}
		}
};

extern GPIO gpio;

#endif
