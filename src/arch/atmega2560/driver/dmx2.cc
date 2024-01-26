/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include "arch.h"
#include "driver/dmx2.h"
#include "driver/gpio.h"

#undef BAUD
#define BAUD 250000UL
#include <util/setbaud.h>

void DMX2::setup()
{
	UBRR2H = UBRRH_VALUE;
	UBRR2L = UBRRL_VALUE;

#if USE_2X
	UCSR2A |= _BV(U2X2);
#else
	UCSR2A &= ~_BV(U2X2);
#endif

	UCSR2B = _BV(TXEN2);
	UCSR2C = _BV(USBS2) | _BV(UCSZ21) | _BV(UCSZ20); // MSB first, 8 data bits, 2 stop bits, no parity
}

void DMX2::write()
{
	// Disable UART for reset and mark signals
	UCSR2B &= ~_BV(TXEN2);
	gpio.output(GPIO::ph1, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::ph1, 1);
	arch.delay_us(8); // mark
	UCSR2B |= _BV(TXEN2); // causes line to go high
	for (uint8_t i = 0; i < 32; i++) {
		while (!(UCSR2A & _BV(UDRE2)));
		UDR2 = frames[i];
	}
	for (uint8_t i = 0; i < 258 - num_frames; i++) {
		while (!(UCSR2A & _BV(UDRE2)));
		UDR2 = 0;
	}
	for (uint8_t i = 0; i < 255; i++) {
		while (!(UCSR2A & _BV(UDRE2)));
		UDR2 = 0;
	}
}

DMX2 dmx2;
