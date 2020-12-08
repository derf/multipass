/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include <time.h>
#include <unistd.h>

#if defined(WITH_LOOP) || defined(TIMER_S)
#include "driver/uptime.h"
void loop();
#endif
#ifdef WITH_WAKEUP
void wakeup();
#endif

void Arch::setup(void) { }

void Arch::idle_loop(void)
{
	while (1) {
		sleep(1);
#ifdef WITH_LOOP
		loop();
#endif
#ifdef WITH_WAKEUP
		wakeup();
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}

void Arch::idle(void)
{
}

void Arch::delay_us(unsigned int const us)
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = us * 1000;
	nanosleep(&ts, NULL);
}

void Arch::delay_ms(unsigned int const ms)
{
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

Arch arch;
