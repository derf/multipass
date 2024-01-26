/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin2.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void StandardInput2::setup()
{
	UCSR2B |= _BV(RXCIE2);
}

bool StandardInput2::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char StandardInput2::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= bufsize;
	return ret;
}

StandardInput2 kin2;

ISR(USART2_RX_vect)
{
	kin2.addKey(UDR2);
}
