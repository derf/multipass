/*
 * Copyright 2022 Daniel Friesel
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
		}
		inline void led_on(unsigned char id = 0) {
		}
		inline void led_off(unsigned char id = 0) {
		}
		inline void led_toggle(unsigned char id = 0) {
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
