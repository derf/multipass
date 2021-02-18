/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

extern "C" {
#include "lm4f_cpp_wrapper.h"
}

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			pa_0 = 0, pa_1, pa_2, pa_3, pa_4, pa_5, pa_6, pa_7,
			pb_0, pb_1, pb_2, pb_3, pb_4, pb_5, pb_6, pb_7,
			pc_0, pc_1, pc_2, pc_3, pc_4, pc_5, pc_6, pc_7,
			pd_0, pd_1, pd_2, pd_3, pd_4, pd_5, pd_6, pd_7,
			pe_0, pe_1, pe_2, pe_3, pe_4, pe_5, pe_6, pe_7,
			pf_0, pf_1, pf_2, pf_3, pf_4,
			PIN_INVALID
		};

		inline void setup() {
			mp_gpio_setup();
		}
		inline void led_on(unsigned char id) {
			mp_gpio_write(pf_1 + id, 1);
		}
		inline void led_off(unsigned char id) {
			mp_gpio_write(pf_1 + id, 0);
		}
		inline void led_toggle(unsigned char id) {
			mp_gpio_write(pf_1 + id, !mp_gpio_read(pf_1 + id));
		}
		inline void input(unsigned char const pin) {
			mp_gpio_input(pin);
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
		}
		inline void output(unsigned char const pin) {
			mp_gpio_output(pin);
		}
		/*
		inline void output(unsigned char const pin, unsigned char const value) {
		}*/
		inline unsigned int read(unsigned char const pin) {
			return mp_gpio_read(pin);
		}
		inline void write(unsigned char const pin, unsigned char value) {
			mp_gpio_write(pin, value);
		}
};

extern GPIO gpio;

#endif
