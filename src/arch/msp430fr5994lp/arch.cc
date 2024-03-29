/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include <msp430.h>

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	PJSEL0 = BIT4 | BIT5;

	PM5CTL0 &= ~LOCKLPM5;

	/*
	 * Note: arch drivers assume SMCLK freq == F_CPU
	 */

#if F_CPU == 16000000UL
	FRCTL0 = FWPW; // unlock FRAM Control
	FRCTL0_L = 0x10; // one wait state before FRAM access (required for 8MHz < F_CPU <= 16 MHz)
	FRCTL0_H = 0xff; // lock FRAM control by writing an invalid password

	// 16MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCORSEL | DCOFSEL_4;
#elif F_CPU == 8000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_6;
#elif F_CPU == 4000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_3;
#elif F_CPU == 1000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_0;
#elif F_CPU == 32768UL
	CSCTL0_H = CSKEY >> 8;
#else
#error Unsupported F_CPU
#endif

#ifdef CONFIG_loop
	CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
#else
	CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
#endif
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
	CSCTL0_H = 0;


#if defined(CONFIG_loop) || F_CPU == 32768UL
	// enable LXFT for RTC
	CSCTL0_H = CSKEY >> 8;
	CSCTL4 &= ~LFXTOFF;
	while (CSCTL5 & LFXTOFFG) {
		CSCTL5 &= ~LFXTOFFG;
	}
	SFRIFG1 &= ~OFIFG;
	CSCTL0_H = 0;

	__delay_cycles(1000000);
#endif

#ifdef WITH_HFXT_16MHZ
	PJSEL0 |= BIT6 | BIT7;
	CSCTL0_H = CSKEY >> 8;

	// set HFFREQ to 8-16 MHz (HFFREQ=10)
	CSCTL4 = HFXTDRIVE_0 | HFFREQ_2 | (CSCTL4 & 0x0f);

	// enable HFXT for SMCLK
	while (CSCTL5 & HFXTOFFG) {
		CSCTL5 &= ~HFXTOFFG;
	}
	SFRIFG1 &= ~OFIFG;
	CSCTL0_H = 0;

	// wait for it to stabilize
	__delay_cycles(1000000);

	// set MCLK and SMCLK to HFXT
	CSCTL0_H = CSKEY >> 8;
#if F_CPU == 16000000UL
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
#elif F_CPU == 8000000UL
	CSCTL3 = DIVA__1 | DIVS__2 | DIVM__2;
#elif F_CPU == 4000000UL
	CSCTL3 = DIVA__1 | DIVS__4 | DIVM__4;
#elif F_CPU == 1000000UL
	CSCTL3 = DIVA__1 | DIVS__16 | DIVM__16;
#endif
	// SELA__LFXTCLK falls back to VLOCLK if LFXT is not available, but set
	// LFXT_FAULT (LFXTOFFG) in the process. We don't want that.
#if defined(CONFIG_loop) || F_CPU == 32768UL
	CSCTL2 = SELA__LFXTCLK | SELS__HFXTCLK | SELM__HFXTCLK;
#else
	CSCTL2 = SELA__VLOCLK | SELS__HFXTCLK | SELM__HFXTCLK;
#endif
	CSCTL0_H = 0;
#endif

#if F_CPU == 32768UL
	CSCTL0_H = CSKEY >> 8;
	CSCTL2 = SELA__LFXTCLK | SELS__LFXTCLK | SELM__LFXTCLK;
	CSCTL0_H = 0;
#endif

#if defined(CONFIG_loop) || defined(TIMER_S)
	// 1s per wakeup for loop. Independent of SMCLK/F_CPU
	TA1CTL = TASSEL__ACLK | ID__8 | MC__UP;
	TA1EX0 = 0;
	TA1CCR0 = 4096;
	TA1CTL |= TACLR | TAIE;
#endif
}

#ifdef CONFIG_wakeup
extern void wakeup();
#endif

#if defined(CONFIG_loop)
extern void loop();
volatile char run_loop = 0;
#endif

inline static unsigned int irq_enabled()
{
	unsigned int sr;
	__asm__ __volatile__("mov SR, %0" : "=r" (sr) : );
	return sr & GIE;
}

volatile bool sleep_done = false;

// max delay: 262 ms @ 16 MHz
// max delay: 524 ms @  8 MHz
void Arch::sleep_ms(unsigned int const ms)
{
	if (ms == 0) {
		return;
	}
	int int_enabled = irq_enabled();
	sleep_done = false;
#if F_CPU == 16000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 7; // /8 -> /64 (250 kHz)
	TA3CCR0 = ms * 250;
#elif F_CPU == 8000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 7; // /8 -> /64 (125 kHz)
	TA3CCR0 = ms * 125;
#elif F_CPU == 4000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 3; // /4 -> /32 (125 kHz)
	TA3CCR0 = ms * 125;
#elif F_CPU == 1000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 0; // /1 -> /8 (125 kHz)
	TA3CCR0 = ms * 125;
#elif F_CPU == 32768UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 0; // /1 -> /8 (4.096 kHz)
	TA3CCR0 = ms * 4;
#else
#error Unsupported F_CPU
#endif /* F_CPU */
	TA3CCTL0 = CCIE;
	TA3CTL |= MC__UP | TACLR;
	while (!sleep_done) {
		asm volatile("nop");
		__bis_SR_register(GIE | LPM2_bits);
		asm volatile("nop");
		if (!int_enabled) {
			__dint();
		}
	}
	TA3CTL = TASSEL__SMCLK;
}

void Arch::delay_us(unsigned int const us)
{
	if (us < 10) {
		for (unsigned int i = 0; i < us; i++) {
			__delay_cycles(F_CPU / 1000000UL);
		}
	} else {
		for (unsigned int i = 0; i < us/10; i++) {
			__delay_cycles(F_CPU / 100000UL);
		}
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		__delay_cycles(F_CPU / 1000UL);
	}
}

void Arch::idle_loop(void)
{
	while (1) {
		asm volatile("nop");
		__bis_SR_register(GIE | LPM2_bits);
		asm volatile("nop");
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
	asm volatile("nop");
	__bis_SR_register(GIE | LPM2_bits);
	asm volatile("nop");
#ifdef CONFIG_wakeup
	wakeup();
#endif
}

Arch arch;

#if defined(CONFIG_loop) || defined(TIMER_S)

#include "driver/uptime.h"

#ifndef __acweaving
// overflow interrupts end up in A1 (joint interrupt for CCR1 ... CCR6 and overflow)
__attribute__((interrupt(TIMER1_A1_VECTOR))) __attribute__((wakeup)) void handle_timer1_overflow()
{
	if (TA1IV == 0x0e) {
#ifdef CONFIG_loop
		run_loop = 1;
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}
#endif

#endif /* defined(CONFIG_loop) || defined(TIMER_S) */

#ifndef __acweaving
// CCR0 interrupts are exclusive to A0
__attribute__((interrupt(TIMER3_A0_VECTOR))) __attribute__((wakeup)) void handle_timer3_ccr0()
{
	sleep_done = true;
}
#endif /* __acweaving */
