/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"

#ifdef TIMER_S
#include "driver/uptime.h"
#endif

extern "C" {
#include "lm4f_cpp_wrapper.h"
};

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
	arch_clock_init();
#if defined(WITH_LOOP)
	arch_init_loop();
#endif
}

#ifdef WITH_WAKEUP
extern void wakeup();
#endif

#if defined(WITH_LOOP)
extern void loop();
extern volatile char run_loop;
#endif

// for 80 MHz
void Arch::delay_us(unsigned int const us)
{
	volatile int x = us * 77;
	while (x--) {
		__asm("nop");
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		volatile int x = 10000;
		while (x--) {
			__asm("nop");
		}
	}
}

void Arch::idle_loop(void)
{
	while (1) {
		__asm__("wfi");
#ifdef WITH_LOOP
		if (run_loop) {
#ifdef TIMER_S
			uptime.tick_s();
#endif
			loop();
			run_loop = 0;
		}
#endif
	}
}

void Arch::idle(void)
{
	__asm__("wfi");
#ifdef WITH_WAKEUP
	wakeup();
#endif
}

Arch arch;
