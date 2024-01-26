/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout2.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#undef BAUD
#define BAUD CONFIG_arch_atmega2560_uart2_baud

#include <util/setbaud.h>

void StandardOutput2::setup()
{
	UBRR2H = UBRRH_VALUE;
	UBRR2L = UBRRL_VALUE;

#if USE_2X
	UCSR2A |= _BV(U2X2);
#else
	UCSR2A &= ~_BV(U2X2);
#endif

	UCSR2B |= _BV(RXEN2) | _BV(TXEN2);
	UCSR2C = _BV(UCSZ21) | _BV(UCSZ20);
}

void StandardOutput2::put(char c)
{
	while (!(UCSR2A & _BV(UDRE2)));
	UDR2 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput2 kout2;
