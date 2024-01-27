/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <msp430.h>
#include "arch.h"
#include "driver/dmx2.h"
#include "driver/gpio.h"

/*
 * Baud rate calculation according to datasheet:
 * N := f_{BRCLK} / Baudrate = F_CPU / 115200 in our case
 * if N <= 16: OS16 = 0, UCBR0 = int(N)
 * if N > 16: OS16 = 1, UCBR0 = int(N/16), UCBRF0 = int(((n/16) - int(n/16)) * 16) = int(N)%16
 * Set UCBRS0 according to table 21-4
 */

void DMX2::setup()
{
	UCA2CTLW0 |= UCSWRST;
#if F_CPU == 16000000UL
	// 16M / 250000 == 64 -> UCOS16 = 1, UCBR0 = 4, UCBRF0 = 0, UCBRS0 = 0x00
	UCA2CTLW0 = UCSWRST | UCSPB | UCSSEL__SMCLK; // MSB first, 8 data bits, 2 stop bits
	UCA2MCTLW = UCOS16;
	UCA2BR0 = 4;
#else
#error Unsupported F_CPU
#endif

	UCA2IRCTL = 0;
	UCA2ABCTL = 0;

	P5SEL0 |= BIT4;
	P5SEL1 &= ~BIT4;
	P5DIR |= BIT4;

	UCA2CTLW0 &= ~UCSWRST;
}

void DMX2::write()
{
	// Disable UART for reset and mark signals
	UCA2CTLW0 |= UCSWRST;
	P5SEL0 &= ~BIT4;
	gpio.output(GPIO::p5_4, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::p5_4, 1);
	arch.delay_us(8); // mark
	P5SEL0 |= BIT4;
	UCA2CTLW0 &= ~UCSWRST; // causes line to go high
	for (unsigned short i = 0; i < num_frames; i++) {
		while (!(UCA2IFG & UCTXIFG));
		UCA2TXBUF = frames[i];
	}
	for (unsigned short i = 0; i < 258 - num_frames; i++) {
		while (!(UCA2IFG & UCTXIFG));
		UCA2TXBUF = 0;
	}
}

DMX2 dmx2;
