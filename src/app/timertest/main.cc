/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/timer.h"
#include "driver/uptime.h"

#ifndef F_TIMER
#define F_TIMER 100000
#endif

volatile unsigned long int timer_val = 0;

void loop(void)
{
	gpio.led_toggle(1);
	kout << "Actual frequency: approx. " << timer_val << " Hz" << endl;
	timer_val = 0;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
#if F_TIMER > 999
	timer.setup_khz(F_TIMER / 1000);
#else
	timer.setup_hz(F_TIMER);
#endif
	timer.start(1);

	gpio.led_on(0);
	kout << "Timer running at " << dec << F_TIMER << " Hz" << endl;

	arch.idle_loop();

	return 0;
}

ON_TIMER_INTERRUPT_head
	gpio.led_toggle(0);
	timer_val++;
ON_TIMER_INTERRUPT_tail
