#include "driver/stdout.h"
#include <avr/io.h>

#ifndef BAUD
#define BAUD 9600UL
#endif

#include <util/setbaud.h>

void StandardOutput::setup()
{
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;

#if USE_2X
	UCSR1A |= _BV(U2X1);
#else
	UCSR1A &= ~_BV(U2X1);
#endif

	UCSR1B |= _BV(RXEN1) | _BV(TXEN1);
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // async UART, 8N1
	//UCSR1D = 0;
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
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = c;
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
