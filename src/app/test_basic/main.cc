/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

void loop(void)
{
	kout << "Loop" << endl;
	gpio.led_toggle(0);
#ifdef TIMER_S
	kout << dec << uptime.get_s() << endl;
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	/*
	 * There may be some delay between program start and UART capture start.
	 */
	for (uint8_t i = 0; i < 255; i++) {
		kout << "Hello, World!" << endl;
	}

	arch.idle_loop();

	return 0;
}
