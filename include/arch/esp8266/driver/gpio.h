/*
 * Copyright 2020 Daniel Friesel
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
			d3 = 0, tx, d4, rx, d2, d1,
			d6 = 12, d7, d5, d8,
			d0 = 16
		};

		void setup();
		void led_on(unsigned char id);
		void led_off(unsigned char id);
		void led_toggle(unsigned char id);
		void input(unsigned char const pin);
		void input(unsigned char const pin, bool pullup);
		void output(unsigned char const pin);
		unsigned char read(unsigned char const pin);
		void write(unsigned char const pin, unsigned char value);
};

extern GPIO gpio;

#endif
