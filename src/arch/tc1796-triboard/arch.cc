/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"

extern "C" {
#include <machine/wdtcon.h>
#include <tc1796b/scu.h>
#include <tc1796b/pmi.h>
}

#ifdef __acweaving
#define __delay_cycles(x)
#endif

#define OF_BYP 29
#define OF_NDIV 16
#define OF_PDIV 13
#define OF_KDIV 8
#define OF_VCOSEL 6
#define OF_SYSFS 2

#define STM_CLC (*(volatile unsigned int*)0xf0000200)
#define STM_TIM5 (*(volatile unsigned int*)0xf0000224)

void Arch::setup(void)
{
	/*
	 * 20 MHz Crystal -> 150 MHz clock
	 * PLL_CLC := (NDIV = 29; PDIV = 0; KDIV = 3; VCOSEL = 2)
	 */
	unlock_wdtcon();
	(*(unsigned int*)0xf0000040) = (29 << OF_NDIV) | (0 << OF_PDIV) | (3 << OF_KDIV) | (2 << OF_VCOSEL);
	STM_CLC = 0x00000100;
	lock_wdtcon();
}

#if defined(CONFIG_loop)
extern void loop();
unsigned int old_tim5 = 0;
#endif

volatile bool sleep_done = false;

void Arch::sleep_ms(unsigned int const ms)
{
	delay_ms(ms);
}

void Arch::delay_us(unsigned int const us)
{
	for (unsigned int i = 0; i < us; i++) {
		for (unsigned int c = 0; c < F_CPU/1000000; c++) {
			asm volatile("nop");
		}
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		for (unsigned int c = 0; c < F_CPU/1000; c++) {
			asm volatile("nop");
		}
	}
}

void Arch::idle_loop(void)
{
	while (1) {
#if defined(CONFIG_loop)
		// STM_TIM5 will overflow once every 1.9 years.
		if ((STM_TIM5 - old_tim5 > 70) || (old_tim5 > STM_TIM5)) {
			old_tim5 = STM_TIM5;
			loop();
		}
#endif
	}
}

void Arch::idle(void)
{
}

Arch arch;
