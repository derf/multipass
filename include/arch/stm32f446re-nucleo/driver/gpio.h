/*
 * Copyright 2020 Birte Kristina Friesel
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
			pa0 = 0, pa1, pa2, pa3, pa4, pa5, pa6, pa7,
			pa8, pa9, pa10, pa11, pa12, pa13, pa14, pa15,
			pb0, pb1, pb2, pb3, pb4, pb5, pb6, pb7,
			pb8, pb9, pb10, pb11, pb12, pb13, pb14, pb15,
			pc0, pc1, pc2, pc3, pc4, pc5, pc6, pc7,
			pc8, pc9, pc10, pc11, pc12, pc13, pc14, pc15,
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
		inline void led_on(unsigned char id = 0) {
			gpio_set(GPIOA, GPIO5);
		}
		inline void led_off(unsigned char id = 0) {
			gpio_clear(GPIOA, GPIO5);
		}
		inline void led_toggle(unsigned char id = 0) {
			gpio_toggle(GPIOA, GPIO5);
		}
#pragma GCC diagnostic pop
		inline void input(unsigned char const pin) {
			if (pin < pb0) {
				gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << pin);
			} else if (pin < pc0) {
				gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pb0));
			} else if (pin < PIN_INVALID) {
				gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pc0));
			}
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
		}
		inline void output(unsigned char const pin) {
			if (pin < pb0) {
				gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << pin);
			} else if (pin < pc0) {
				gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pb0));
			} else if (pin < PIN_INVALID) {
				gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pc0));
			}
		}
		/*
		inline void output(unsigned char const pin, unsigned char const value) {
		}*/
		inline unsigned int read(unsigned char const pin) {
			if (pin < pb0) {
				return gpio_get(GPIOA, 1 << pin);
			} else if (pin < pc0) {
				return gpio_get(GPIOB, 1 << (pin-pb0));
			} else if (pin < PIN_INVALID) {
				return gpio_get(GPIOC, 1 << (pin-pc0));
			}
		}
		inline void write(unsigned char const pin, unsigned char value) {
			if (pin < pb0) {
				if (value) {
					gpio_port_write(GPIOA, gpio_port_read(GPIOA) | (1 << pin));
				} else {
					gpio_port_write(GPIOA, gpio_port_read(GPIOA) & ~(1 << pin));
				}
			} else if (pin < pc0) {
				if (value) {
					gpio_port_write(GPIOB, gpio_port_read(GPIOB) | (1 << (pin-pb0)));
				} else {
					gpio_port_write(GPIOB, gpio_port_read(GPIOB) & ~(1 << (pin-pb0)));
				}
			} else if (pin < PIN_INVALID) {
				if (value) {
					gpio_port_write(GPIOC, gpio_port_read(GPIOC) | (1 << (pin-pc0)));
				} else {
					gpio_port_write(GPIOC, gpio_port_read(GPIOC) & ~(1 << (pin-pc0)));
				}
			}
		}
};

extern GPIO gpio;

#endif
