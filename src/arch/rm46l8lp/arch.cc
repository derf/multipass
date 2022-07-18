/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "gio.h"
#include "rti.h"

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
	gioInit();

	// disable counter blocks
	rtiREG1->GCTRL = 0;

	// internal clock, RTIUC0 -> RTIFRC0
	rtiREG1->TBCTRL = 0;

	// no capture sources
	rtiREG1->CAPCTRL = 0;

	// input source: counter 0 -> compare 0/1, counter 1 -> compare 2/3
	rtiREG1->COMPCTRL = 0x00001100U;

	// reset up / free running counter 0
	rtiREG1->CNT[0].UCx = 0;
	rtiREG1->CNT[0].FRCx = 0;

	// reset up / free running counter 1
	rtiREG1->CNT[1].UCx = 0;
	rtiREG1->CNT[1].FRCx = 0;

	// free running counter 0 freq = RTICLK/2^32
	rtiREG1->CNT[0].CPUCx = 0xffffffffU;

	// free running counter 1 freq = RTICLK/2 ( == VCLK/4, see halcogen/system.c)
	rtiREG1->CNT[1].CPUCx = 1;

	// one interrupt per second
	rtiREG1->CMP[2].COMPx = F_CPU/4;
	rtiREG1->CMP[2].UDCPx = F_CPU/4;

	// clear all pending interrupts
	rtiREG1->INTFLAG = 0x0007000FU;

	// disable all interrupts
	rtiREG1->CLEARINTENA = 0x00070F0FU;

#if defined(CONFIG_loop) || defined(TIMER_S)
	rtiEnableNotification(rtiNOTIFICATION_COMPARE2);
#endif
	_enable_IRQ();
#if defined(CONFIG_loop) || defined(TIMER_S)
	rtiStartCounter(rtiCOUNTER_BLOCK1);
#endif
}

#ifdef CONFIG_wakeup
extern void wakeup();
#endif

#if defined(CONFIG_loop)
extern void loop();
volatile bool run_loop = 0;
#endif

void Arch::delay_us(unsigned int const us)
{
	if (us < 10) {
		for (unsigned int i = 0; i < us; i++) {
			__delay_cycles(F_CPU / 4000000UL);
		}
	} else {
		for (unsigned int i = 0; i < us/10; i++) {
			__delay_cycles(F_CPU / 400000UL);
		}
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		__delay_cycles(F_CPU / 4000UL);
	}
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
	}
}

void Arch::idle(void)
{
#ifdef CONFIG_wakeup
	wakeup();
#endif
}

#if defined(CONFIG_loop) || defined(TIMER_S)
#include "driver/uptime.h"
void rtiNotification(uint32 notification)
{
	if (notification == rtiNOTIFICATION_COMPARE2) {
#ifdef CONFIG_loop
		run_loop = 1;
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}
#endif

Arch arch;
