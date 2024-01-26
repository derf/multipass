/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void StandardInput1::setup()
{
	UCSR1B |= _BV(RXCIE1);
}

bool StandardInput1::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char StandardInput1::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= bufsize;
	return ret;
}

StandardInput1 kin1;

ISR(USART1_RX_vect)
{
	kin1.addKey(UDR1);
}
