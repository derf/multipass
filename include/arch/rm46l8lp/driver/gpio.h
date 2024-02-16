/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

#include "gio.h"

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			a0 = 0, a1, a2, a3, a4, a5, a6, a7,
			b0, b1, b2, b3, b4, b5, b6, b7,
			PIN_INVALID
		};

		inline void setup() {
			/*
			 * Default configuration:
			 * A0 A1 A2 b1 B2 as output, others as input.
			 */
			gioInit();
		}
		inline void led_on(unsigned char id = 0) {
			if (id == 0) {
				gioPORTB->DSET = 1U << 1;
			} else {
				gioPORTB->DSET = 1U << 2;
			}
		}
		inline void led_off(unsigned char id = 0) {
			if (id == 0) {
				gioPORTB->DCLR = 1U << 1;
			} else {
				gioPORTB->DCLR = 1U << 2;
			}
		}
		inline void led_toggle(unsigned char id = 0) {
			if (id == 0) {
				gioToggleBit(gioPORTB,1);
			} else {
				gioToggleBit(gioPORTB,2);
			}
		}
		inline void input(unsigned char const pin) {
			if (pin < b0) {
				gioPORTA->DIR &= ~(1U << pin);
			} else if (pin < PIN_INVALID) {
				gioPORTB->DIR &= ~(1U << (pin - b0));
			}
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
			// TODO
		}
		inline void output(unsigned char const pin) {
			if (pin < b0) {
				gioPORTA->DIR |= 1U << pin;
			} else if (pin < PIN_INVALID) {
				gioPORTB->DIR |= 1U << (pin - b0);
			}
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			write(pin, value);
			output(pin);
		}
		inline unsigned char read(unsigned char const pin) {
			if (pin < b0) {
				return gioPORTA->DIN & (1U << pin);
			} else if (pin < PIN_INVALID) {
				return gioPORTB->DIN & (1U << (pin - b0));
			}
			return 0;
		}
		inline void write(unsigned char const pin, unsigned char value) {
			if (pin < b0) {
				if (value) {
					gioPORTA->DSET = 1U << pin;
				} else {
					gioPORTA->DCLR = 1U << pin;
				}
			} else if (pin < PIN_INVALID) {
				if (value) {
					gioPORTB->DSET = 1U << (pin - b0);
				} else {
					gioPORTB->DCLR = 1U << (pin - b0);
				}
			}
		}
		inline void write_mask(unsigned char const pin_base, unsigned char set_mask, unsigned char clear_mask) {
			// TODO
		}
};

extern GPIO gpio;

#endif
