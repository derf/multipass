/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void Arch::setup(void)
{
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
void loop();

#endif

void Arch::idle_loop(void)
{
	while (1) {
		SMCR = _BV(SE);
		asm("sleep");
		SMCR = 0;
		asm("wdr");
#ifdef CONFIG_loop
		loop();
#endif
#ifdef CONFIG_wakeup
		wakeup();
#endif
#ifdef TIMER_S
		uptime.tick_s();
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

ISR(TIMER1_COMPA_vect)
{
}

#endif
