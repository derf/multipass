/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#undef BAUD
#define BAUD CONFIG_arch_atmega2560_uart1_baud

#include <util/setbaud.h>

void StandardOutput1::setup()
{
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;

#if USE_2X
	UCSR1A |= _BV(U2X1);
#else
	UCSR1A &= ~_BV(U2X1);
#endif

	UCSR1B |= _BV(RXEN1) | _BV(TXEN1);
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
}

void StandardOutput1::put(char c)
{
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput1 kout1;
