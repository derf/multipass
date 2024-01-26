/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include "arch.h"
#include "driver/dmx1.h"
#include "driver/gpio.h"

#undef BAUD
#define BAUD 250000UL
#include <util/setbaud.h>

void DMX1::setup()
{
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;

#if USE_2X
	UCSR1A |= _BV(U2X1);
#else
	UCSR1A &= ~_BV(U2X1);
#endif

	UCSR1B = _BV(TXEN1);
	UCSR1C = _BV(USBS1) | _BV(UCSZ11) | _BV(UCSZ10); // MSB first, 8 data bits, 2 stop bits, no parity
}

void DMX1::write()
{
	// Disable UART for reset and mark signals
	UCSR1B &= ~_BV(TXEN1);
	gpio.output(GPIO::pd3, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::pd3, 1);
	arch.delay_us(8); // mark
	UCSR1B |= _BV(TXEN1); // causes line to go high
	for (uint8_t i = 0; i < 32; i++) {
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = frames[i];
	}
	for (uint8_t i = 0; i < 258 - num_frames; i++) {
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = 0;
	}
	for (uint8_t i = 0; i < 255; i++) {
		while (!(UCSR1A & _BV(UDRE1)));
		UDR1 = 0;
	}
}

DMX1 dmx1;
