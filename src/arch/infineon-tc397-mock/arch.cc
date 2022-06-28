/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
}

#ifdef CONFIG_wakeup
extern void wakeup();
#endif

#if defined(CONFIG_loop)
extern void loop();
volatile char run_loop = 0;
#endif

volatile bool sleep_done = false;

void Arch::sleep_ms(unsigned int const ms)
{
}

void Arch::delay_us(unsigned int const us)
{
}
void Arch::delay_ms(unsigned int const ms)
{
}

void Arch::idle_loop(void)
{
	while (1) {
#if defined(CONFIG_loop)
		if (run_loop) {
			loop();
			run_loop = 0;
		}
#endif
#ifdef CONFIG_wakeup
		wakeup();
#endif
	}
}

void Arch::idle(void)
{
#ifdef CONFIG_wakeup
	wakeup();
#endif
}

Arch arch;
