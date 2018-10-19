#include "driver/stdout.h"
#include <avr/io.h>
#include <avr/interrupt.h>

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

void StandardOutput::put(char c)
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
