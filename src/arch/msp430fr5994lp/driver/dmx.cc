/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <msp430.h>
#include "arch.h"
#include "driver/dmx.h"
#include "driver/gpio.h"

/*
 * Baud rate calculation according to datasheet:
 * N := f_{BRCLK} / Baudrate = F_CPU / 115200 in our case
 * if N <= 16: OS16 = 0, UCBR0 = int(N)
 * if N > 16: OS16 = 1, UCBR0 = int(N/16), UCBRF0 = int(((n/16) - int(n/16)) * 16) = int(N)%16
 * Set UCBRS0 according to table 21-4
 */

void DMX::setup()
{
	UCA3CTLW0 |= UCSWRST;
#if F_CPU == 16000000UL
	// 16M / 250000 == 64 -> UCOS16 = 1, UCBR0 = 4, UCBRF0 = 0, UCBRS0 = 0x00
	UCA3CTLW0 = UCSWRST | UCSPB | UCSSEL__SMCLK; // MSB first, 8 data bits, 2 stop bits
	UCA3MCTLW = UCOS16;
	UCA3BR0 = 4;
#else
#error Unsupported F_CPU
#endif

	UCA3IRCTL = 0;
	UCA3ABCTL = 0;

	P6SEL0 |= BIT0;
	P6SEL1 &= ~BIT0;
	P6DIR |= BIT0;

	UCA3CTLW0 &= ~UCSWRST;
}

void DMX::write()
{
	// Disable UART for reset and mark signals
	UCA3CTLW0 |= UCSWRST;
	P6SEL0 &= ~BIT0;
	gpio.output(GPIO::p2_5, 0);
	arch.delay_us(88); // break / reset
	gpio.output(GPIO::p2_5, 1);
	arch.delay_us(8); // mark
	P6SEL0 |= BIT0;
	UCA3CTLW0 &= ~UCSWRST; // causes line to go high
	for (unsigned char i = 0; i < 16; i++) {
		while (!(UCA3IFG & UCTXIFG));
		UCA3TXBUF = frames[i];
	}
	for (unsigned char i = 0; i < 241; i++) {
		while (!(UCA3IFG & UCTXIFG));
		UCA3TXBUF = 0;
	}
	for (unsigned char i = 0; i < 255; i++) {
		while (!(UCA3IFG & UCTXIFG));
		UCA3TXBUF = 0;
	}
}

DMX dmx;
