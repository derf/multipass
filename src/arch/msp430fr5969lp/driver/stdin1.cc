/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin1.h"
#include <msp430.h>

void StandardInput1::setup()
{
	UCA1CTLW0 |= UCSWRST;
	P2SEL0 &= ~BIT6;
	P2SEL1 |= BIT6;
	UCA1CTLW0 &= ~UCSWRST;
	UCA1IE |= UCRXIE;
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

__attribute__((interrupt(USCI_A1_VECTOR))) __attribute__((wakeup)) void handle_stdin1()
{
	if (UCA1IFG & UCRXIFG) {
		kin1.addKey(UCA1RXBUF);
	}
}
