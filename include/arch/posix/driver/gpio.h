/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);
		unsigned int pin_dir, pin_pull, pin_out, pull_dir;
#ifdef GPIO_TRACE
		void print_pinstate() {
			unsigned int mask = 1;
			for (unsigned char i=0; i < 32; i++) {
				if (i == 0) {
					fputs("▶ LED ", stdout);
				}
				else if (i == 8) {
					fputs("  GPIO ", stdout);
				}
				else if (i == 16) {
					fputs("  ", stdout);
				}
				else if (i == 24) {
					fputs("  ", stdout);
				}
				if (pin_dir & mask) {
					if (pin_out & (1<<i)) {
						fputs("█", stdout);
					} else {
						putc(' ', stdout);
					}
				} else {
					if (pin_pull & mask) {
						if (pull_dir & mask) {
							fputs("↑", stdout);
						} else {
							fputs("↓", stdout);
						}
					} else {
						fputs("·", stdout);
					}
				}
				mask <<= 1;
			}
			putc('\n', stdout);
		}
#else
		inline void print_pinstate() {}
#endif

	public:
		GPIO () : pin_dir(0), pin_pull(0), pin_out(0), pull_dir(0) {}

		enum Pin : unsigned char {
			pl0 = 0, pl1, pl2, pl3, pl4, pl5, pl6, pl7,
			px00, px01, px02, px03, px04, px05, px06, px07,
			px08, px09, px10, px11, px12, px13, px14, px15,
			px16, px17, px18, px19, px20, px21, px22, px23
		};

		inline void setup() {
			pin_dir |= 0x0000ff;
		}
		inline void led_on(unsigned char id = 0) {
			if (id <= pl7) {
				pin_out |= (1 << id);
			}
			print_pinstate();
		}
		inline void led_off(unsigned char id = 0) {
			if (id <= pl7) {
				pin_out &= ~(1 << id);
			}
			print_pinstate();
		}
		inline void led_toggle(unsigned char id = 0) {
			if (id <= pl7) {
				if (pin_out & (1 << id)) {
					led_off(id);
				} else {
					led_on(id);
				}
			}
		}
		inline void input(unsigned char const pin) {
			pin_dir &= ~(1<<pin);
			print_pinstate();
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
			pin_dir &= ~(1<<pin);
			pin_pull |= (1<<pin);
			if (pull) {
				pull_dir |= (1<<pin);
			} else {
				pull_dir &= ~(1<<pin);
			}
			print_pinstate();
		}
		inline void output(unsigned char const pin) {
			pin_dir |= (1<<pin);
			print_pinstate();
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			pin_dir |= (1<<pin);
			if (value)
				pin_out |= (1<<pin);
			else
				pin_out &= ~(1<<pin);
			print_pinstate();
		}
#pragma GCC diagnostic ignored "-Wunused-parameter"
		inline unsigned char read(unsigned char const pin) {
			return 1;
		}
#pragma GCC diagnostic pop
		inline void write(unsigned char const pin, unsigned char value) {
			if (value)
				pin_out |= (1<<pin);
			else
				pin_out &= ~(1<<pin);
			print_pinstate();
		}
		inline void enable_int() {}
		inline void disable_int() {}
};

extern GPIO gpio;

#endif
