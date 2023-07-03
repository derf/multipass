/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"
#include <avr/io.h>

#ifndef BAUD
#define BAUD 9600UL
#endif

#include <util/setbaud.h>

void StandardOutput::setup()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif

	UCSR0B |= _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

OutputStream & StandardOutput::pprint(const char *text)
{
	PGM_P p = reinterpret_cast<PGM_P>(text);
	char c;
	while ((c = pgm_read_byte(p++))) {
		put(c);
	}
	return *this;
}

void StandardOutput::put(char c)
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
