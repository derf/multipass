/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			PIN_INVALID
		};

		inline void setup() {
			*((int*)0xf0000f10) = 0x80808080;
			*((int*)0xf0000f14) = 0x80808080;
			*((int*)0xf0000f00) = 0x000000ff;
		}
		inline void led_on(unsigned char id = 0) {
			*((int*)0xf0000f00) &= ~(1 << id);
		}
		inline void led_off(unsigned char id = 0) {
			*((int*)0xf0000f00) |= (1 << id);
		}
		inline void led_toggle(unsigned char id = 0) {
			*((int*)0xf0000f00) ^= (1 << id);
		}
		inline void input(unsigned char const pin) {
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
		}
		inline void output(unsigned char const pin) {
		}
		inline void output(unsigned char const pin, unsigned char const value) {
		}
		inline unsigned char read(unsigned char const pin) {
			return 0;
		}
		inline void write(unsigned char const pin, unsigned char value) {
		}
		inline void write_mask(unsigned char const pin_base, unsigned char set_mask, unsigned char clear_mask) {
		}
};

extern GPIO gpio;

#endif
