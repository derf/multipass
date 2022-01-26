/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include "arch.h"
#include "driver/dmx.h"
#include "driver/gpio.h"

#undef BAUD
#define BAUD 250000UL
#include <util/setbaud.h>

void DMX::setup()
{
	UBRR3H = UBRRH_VALUE;
	UBRR3L = UBRRL_VALUE;

#if USE_2X
	UCSR3A |= _BV(U2X3);
#else
	UCSR3A &= ~_BV(U2X3);
#endif

	UCSR3B = _BV(TXEN3);
	UCSR3C = _BV(USBS3) | _BV(UCSZ31) | _BV(UCSZ30); // MSB first, 8 data bits, 2 stop bits, no parity
}

void DMX::write()
{
	// Disable UART for reset and mark signals
	UCSR3B &= ~_BV(TXEN3);
	gpio.output(GPIO::pj1, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::pj1, 1);
	arch.delay_us(8); // mark
	UCSR3B |= _BV(TXEN3); // causes line to go high
	for (uint8_t i = 0; i < 32; i++) {
		while (!(UCSR3A & _BV(UDRE3)));
		UDR3 = frames[i];
	}
	for (uint8_t i = 0; i < 258 - num_frames; i++) {
		while (!(UCSR3A & _BV(UDRE3)));
		UDR3 = 0;
	}
	for (uint8_t i = 0; i < 255; i++) {
		while (!(UCSR3A & _BV(UDRE3)));
		UDR3 = 0;
	}
}

DMX dmx;
