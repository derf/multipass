/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout3.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#undef BAUD
#define BAUD CONFIG_arch_atmega2560_uart3_baud

#include <util/setbaud.h>

void StandardOutput3::setup()
{
	UBRR3H = UBRRH_VALUE;
	UBRR3L = UBRRL_VALUE;

#if USE_2X
	UCSR3A |= _BV(U2X3);
#else
	UCSR3A &= ~_BV(U2X3);
#endif

	UCSR3B |= _BV(RXEN3) | _BV(TXEN3);
	UCSR3C = _BV(UCSZ31) | _BV(UCSZ30);
}

void StandardOutput3::put(char c)
{
	while (!(UCSR3A & _BV(UDRE3)));
	UDR3 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput3 kout3;
