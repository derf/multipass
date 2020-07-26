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
		/*
		inline void input(unsigned char const pin) {
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
		}
		inline void output(unsigned char const pin) {
		}
		inline void output(unsigned char const pin, unsigned char const value) {
		}
		inline unsigned char read(unsigned char const pin) {
		}
		inline void write(unsigned char const pin, unsigned char value) {
		}
		inline void write_mask(unsigned char const pin_base, unsigned char set_mask, unsigned char clear_mask) {
		}
		*/
};

extern GPIO gpio;

#endif
