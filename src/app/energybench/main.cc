/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.output(GPIO::p1_1);

	arch.delay_ms(5000);

	gpio.write(GPIO::p1_1, 1);
	__delay_cycles(64000); // 8 ms
	__delay_cycles(64000); // 8 ms
	gpio.write(GPIO::p1_1, 0);
	__delay_cycles(64000); // 8 ms
	__delay_cycles(64000); // 8 ms
	gpio.write(GPIO::p1_1, 1);
	__delay_cycles(64000); // 8 ms
	__delay_cycles(64000); // 8 ms
	gpio.write(GPIO::p1_1, 0);

	for (uint8_t i = 0; i < 200; i++) {
		arch.sleep_ms(50);
		gpio.write(GPIO::p1_1, 1);
		arch.sleep_ms(i);
		gpio.write(GPIO::p1_1, 0);
		arch.sleep_ms(i);
		gpio.write(GPIO::p1_1, 1);
		arch.sleep_ms(i);
		gpio.write(GPIO::p1_1, 0);
	}

	for (uint8_t i = 0; i < 200; i++) {
		arch.delay_ms(50);
		gpio.write(GPIO::p1_1, 1);
		arch.delay_ms(i);
		gpio.write(GPIO::p1_1, 0);
		arch.delay_ms(i);
		gpio.write(GPIO::p1_1, 1);
		arch.delay_ms(i);
		gpio.write(GPIO::p1_1, 0);
	}

	while(1) {
		arch.idle();
	}

	return 0;
}
