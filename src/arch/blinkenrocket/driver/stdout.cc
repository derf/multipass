/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void StandardOutput::setup()
{
#ifndef KOUT_NOP
	PORTC |= _BV(PC1);
	DDRC |= _BV(DDC1);
#endif
}

void StandardOutput::put(char c)
{
#ifndef KOUT_NOP
	unsigned char i = 1;
	PORTC &= ~_BV(PC1);
	__builtin_avr_delay_cycles(59);
	while (i < 0x80) {
		if (c & i) {
			PORTC |= _BV(PC1);
		} else {
			PORTC &= ~_BV(PC1);
		}
		i <<= 1;
		__builtin_avr_delay_cycles(54);
	}
	__builtin_avr_delay_cycles(6);
	PORTC &= ~_BV(PC1);
	__builtin_avr_delay_cycles(67);
	PORTC |= _BV(PC1);
	__builtin_avr_delay_cycles(150);
	if (c == '\n') {
		put('\r');
	}
#endif
}

StandardOutput kout;
