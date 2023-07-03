/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include "arch.h"
#include "driver/dmx.h"
#include "driver/gpio.h"

#define BAUD 250000UL
#include <util/setbaud.h>

void DMX::setup()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif

	UCSR0B |= _BV(TXEN0);
	UCSR0C = _BV(USBS0) | _BV(UCSZ01) | _BV(UCSZ00); // MSB first, 8 data bits, 2 stop bits, no parity
}

void DMX::write()
{
	// Disable UART for reset and mark signals
	UCSR0B &= ~_BV(TXEN0);
	gpio.output(GPIO::pd1, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::pd1, 1);
	arch.delay_us(8); // mark
	UCSR0B |= _BV(TXEN0); // causes line to go high
	for (uint8_t i = 0; i < 16; i++) {
		while (!(UCSR0A & _BV(UDRE0)));
		UDR0 = frames[i];
	}
	for (uint8_t i = 0; i < 241; i++) {
		while (!(UCSR0A & _BV(UDRE0)));
		UDR0 = 0;
	}
	for (uint8_t i = 0; i < 255; i++) {
		while (!(UCSR0A & _BV(UDRE0)));
		UDR0 = 0;
	}
}

DMX dmx;
