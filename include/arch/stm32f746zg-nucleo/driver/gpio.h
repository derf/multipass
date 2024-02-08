/*
 * Copyright 2024 Birte Kristina Friesel
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
			pd0, pd1, pd2, pd3, pd4, pd5, pd6, pd7,
			pd8, pd9, pd10, pd11, pd12, pd13, pd14, pd15,
			pe0, pe1, pe2, pe3, pe4, pe5, pe6, pe7,
			pe8, pe9, pe10, pe11, pe12, pe13, pe14, pe15,
			pf0, pf1, pf2, pf3, pf4, pf5, pf6, pf7,
			pf8, pf9, pf10, pf11, pf12, pf13, pf14, pf15,
			pg0, pg1, pg2, pg3, pg4, pg5, pg6, pg7,
			pg8, pg9, pg10, pg11, pg12, pg13, pg14, pg15,
			ph0, ph1, ph2, ph3, ph4, ph5, ph6, ph7,
			ph8, ph9, ph10, ph11, ph12, ph13, ph14, ph15,
			PIN_INVALID
		};

		inline void setup() {
			rcc_periph_clock_enable(RCC_GPIOA);
			rcc_periph_clock_enable(RCC_GPIOB);
			rcc_periph_clock_enable(RCC_GPIOC);
			rcc_periph_clock_enable(RCC_GPIOD);

			// Set LEDs as output
			gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
			gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
			gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);
		}
		inline void led_on(unsigned char id = 0) {
			if (id == 2) {
				gpio_set(GPIOB, GPIO0);
			} else if (id == 1) {
				gpio_set(GPIOB, GPIO7);
			} else {
				gpio_set(GPIOB, GPIO14);
			}
		}
		inline void led_off(unsigned char id = 0) {
			if (id == 2) {
				gpio_clear(GPIOB, GPIO0);
			} else if (id == 1) {
				gpio_clear(GPIOB, GPIO7);
			} else {
				gpio_clear(GPIOB, GPIO14);
			}
		}
		inline void led_toggle(unsigned char id = 0) {
			if (id == 2) {
				gpio_toggle(GPIOB, GPIO0);
			} else if (id == 1) {
				gpio_toggle(GPIOB, GPIO7);
			} else {
				gpio_toggle(GPIOB, GPIO14);
			}
		}
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
