/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdin.h"
#include <msp430.h>

void StandardInput::setup()
{
	UCA0CTLW0 |= UCSWRST;
	P2SEL0 &= ~BIT1;
	P2SEL1 |= BIT1;
	UCA0CTLW0 &= ~UCSWRST;
	UCA0IE |= UCRXIE;
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
	read_pos %= bufsize;
	return ret;
}

StandardInput kin;

__attribute__((interrupt(USCI_A0_VECTOR))) __attribute__((wakeup)) void handle_stdin()
{
	if (UCA0IFG & UCRXIFG) {
		kin.addKey(UCA0RXBUF);
	}
}
