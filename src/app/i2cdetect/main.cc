/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

unsigned int i2c_status[128 / (8 * sizeof(unsigned int)) + 1];

int main(void)
{

	arch.setup();
	gpio.setup();
	kout.setup();

	while (i2c.setup() != 0) {
		kout << "I2C setup FAILED" << endl;
		arch.delay_ms(1000);
	}

	kout << "I2C setup OK" << endl;

	while (1) {
		for (unsigned char i = 0; i < sizeof(i2c_status)/sizeof(unsigned int); i++) {
			i2c_status[i] = 0;
		}

		i2c.scan(i2c_status);

		kout << "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f";
		for (unsigned char address = 0; address < 128; address++) {
			if ((address & 0x0f) == 0) {
				kout << endl;
				kout.printf_uint8(address);
				kout << ":";
			}

			if (i2c_status[address / (8 * sizeof(unsigned int))] & (1 << (address % (8 * sizeof(unsigned int))))) {
				kout << " ";
				kout.printf_uint8(address);
			} else {
				kout << " --";
			}
		}
		kout << endl << endl;
		arch.delay_ms(1000);
	}

	arch.idle_loop();

	return 0;
}
