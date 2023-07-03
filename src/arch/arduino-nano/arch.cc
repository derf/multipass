/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

void Arch::setup(void)
{
	wdt_disable();

#if F_CPU == 16000000UL
	/* default */
#elif F_CPU == 8000000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS0);
#elif F_CPU == 4000000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS1);
#elif F_CPU == 2000000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
#elif F_CPU == 1000000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS2);
#elif F_CPU == 500000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS2) | _BV(CLKPS0);
#elif F_CPU == 250000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS2) | _BV(CLKPS1);
#elif F_CPU == 125000UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS2) | _BV(CLKPS1) | _BV(CLKPS0);
#elif F_CPU == 62500UL
	CLKPR = _BV(CLKPCE);
	CLKPR = _BV(CLKPS3);
#else
#error Unsupported F_CPU
#endif

#if defined(CONFIG_loop) || defined(TIMER_S)
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); // /1024
	OCR1A = F_CPU / 1024;
	TIMSK1 = _BV(OCIE1A);
#endif

	sei();
}

#ifdef CONFIG_wakeup
void wakeup();
#endif

#if defined(CONFIG_loop) || defined(TIMER_S)

#include "driver/uptime.h"

#endif

#if defined(CONFIG_loop)
extern void loop();
volatile char run_loop = 0;
#endif

void Arch::idle_loop(void)
{
	while (1) {
		SMCR = _BV(SE);
		asm("sleep");
		SMCR = 0;
		asm("wdr");
#ifdef CONFIG_loop
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
	SMCR = _BV(SE);
	asm("sleep");
	SMCR = 0;
	asm("wdr");
}

void Arch::delay_us(unsigned int const us)
{
	for (unsigned int i = 0; i < us; i++) {
		_delay_us(1);
	}
}

void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}

Arch arch;

#if defined(CONFIG_loop) || defined(TIMER_S)

#ifndef __acweaving
ISR(TIMER1_COMPA_vect)
{
#ifdef CONFIG_loop
	run_loop = 1;
#endif
#ifdef TIMER_S
	uptime.tick_s();
#endif
}
#endif

#endif
