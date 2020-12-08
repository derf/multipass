/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			pa_0 = 0, pa_1, pa_2, pa_3, pa_4, pa_5, pa_6, pa_7,
			pa_8, pa_9, pa_10, pa_11, pa_12, pa_13, pa_14, pa_15,
			pb_0, pb_1, pb_2, pb_3, pb_4, pb_5, pb_6, pb_7,
			pb_8, pb_9, pb_10, pb_11, pb_12, pb_13, pb_14, pb_15,
			pc_0, pc_1, pc_2, pc_3, pc_4, pc_5, pc_6, pc_7,
			pc_8, pc_9, pc_10, pc_11, pc_12, pc_13, pc_14, pc_15,
			PIN_INVALID
		};

		inline void setup() {
			rcc_periph_clock_enable(RCC_GPIOA);
			rcc_periph_clock_enable(RCC_GPIOB);
			rcc_periph_clock_enable(RCC_GPIOC);

			// Set LED as output
			gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
		}
#pragma GCC diagnostic ignored "-Wunused-parameter"
		inline void led_on(unsigned char id) {
			gpio_set(GPIOA, GPIO5);
		}
		inline void led_off(unsigned char id) {
			gpio_clear(GPIOA, GPIO5);
		}
		inline void led_toggle(unsigned char id) {
			gpio_toggle(GPIOA, GPIO5);
		}
#pragma GCC diagnostic pop
		inline void input(unsigned char const pin) {
			if (pin < pb_0) {
				gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << pin);
			} else if (pin < pc_0) {
				gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pb_0));
			} else if (pin < PIN_INVALID) {
				gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pc_0));
			}
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
		}
		inline void output(unsigned char const pin) {
			if (pin < pb_0) {
				gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << pin);
			} else if (pin < pc_0) {
				gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pb_0));
			} else if (pin < PIN_INVALID) {
				gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pc_0));
			}
		}
		/*
		inline void output(unsigned char const pin, unsigned char const value) {
		}*/
		inline unsigned int read(unsigned char const pin) {
			if (pin < pb_0) {
				return gpio_get(GPIOA, 1 << pin);
			} else if (pin < pc_0) {
				return gpio_get(GPIOB, 1 << (pin-pb_0));
			} else if (pin < PIN_INVALID) {
				return gpio_get(GPIOC, 1 << (pin-pc_0));
			}
		}
		inline void write(unsigned char const pin, unsigned char value) {
			if (pin < pb_0) {
				if (value) {
					gpio_port_write(GPIOA, gpio_port_read(GPIOA) | (1 << pin));
				} else {
					gpio_port_write(GPIOA, gpio_port_read(GPIOA) & ~(1 << pin));
				}
			} else if (pin < pc_0) {
				if (value) {
					gpio_port_write(GPIOB, gpio_port_read(GPIOB) | (1 << (pin-pb_0)));
				} else {
					gpio_port_write(GPIOB, gpio_port_read(GPIOB) & ~(1 << (pin-pb_0)));
				}
			} else if (pin < PIN_INVALID) {
				if (value) {
					gpio_port_write(GPIOC, gpio_port_read(GPIOC) | (1 << (pin-pc_0)));
				} else {
					gpio_port_write(GPIOC, gpio_port_read(GPIOC) & ~(1 << (pin-pc_0)));
				}
			}
		}
};

extern GPIO gpio;

#endif
