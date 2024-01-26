/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin3.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void StandardInput3::setup()
{
	UCSR3B |= _BV(RXCIE3);
}

bool StandardInput3::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char StandardInput3::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= bufsize;
	return ret;
}

StandardInput3 kin3;

ISR(USART3_RX_vect)
{
	kin3.addKey(UDR3);
}
