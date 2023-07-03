/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void StandardInput::setup()
{
	UCSR0B |= _BV(RXCIE0);
}

bool StandardInput::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char StandardInput::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= 8;
	return ret;
}

StandardInput kin;

ISR(USART_RX_vect)
{
	while (UCSR0A & _BV(RXC0)) {
		kin.addKey(UDR0);
	}
}
