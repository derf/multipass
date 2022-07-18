/*
 * Copyright 2022 Daniel Friesel
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
			a_0 = 0, a_1, a_2, a_3, a_4, a_5, a_6, a_7,
			b_0, b_1, b_2, b_3, b_4, b_5, b_6, b_7,
			PIN_INVALID
		};

		inline void setup() {
			gioInit();
		}
		inline void led_on(unsigned char id = 0) {
			if (id == 0) {
				gioSetBit(gioPORTB,1, 1);
			} else {
				gioSetBit(gioPORTB,2, 1);
			}
		}
		inline void led_off(unsigned char id = 0) {
			if (id == 0) {
				gioSetBit(gioPORTB,1, 0);
			} else {
				gioSetBit(gioPORTB,2, 0);
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
			// TODO
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
			// TODO
		}
		inline void output(unsigned char const pin) {
			// TODO
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			// TODO
		}
		inline unsigned char read(unsigned char const pin) {
			// TODO
			return 0;
		}
		inline void write(unsigned char const pin, unsigned char value) {
			// TODO
		}
		inline void write_mask(unsigned char const pin_base, unsigned char set_mask, unsigned char clear_mask) {
			// TODO
		}
};

extern GPIO gpio;

#endif
