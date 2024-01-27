/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout1.h"
#include <msp430.h>

/*
 * Baud rate calculation according to datasheet:
 * N := f_{BRCLK} / Baudrate = F_CPU / 115200 in our case
 * if N <= 16: OS16 = 0, UCBR0 = int(N)
 * if N > 16: OS16 = 1, UCBR0 = int(N/16), UCBRF0 = int(((n/16) - int(n/16)) * 16) = int(N)%16
 * Set UCBRS0 according to table 21-4
 */

void StandardOutput1::setup()
{
	UCA1CTLW0 |= UCSWRST;
#if F_CPU == 16000000UL
	// 16M / 115200 == 138.88889 -> UCOS16 = 1, UCBR0 = 16, UCBRF0 = 10, UCBRS0 = 0xf7 ("0.8751")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = UCOS16 | (10<<4) | 0xF700;
	UCA1BR0 = 8;
#elif F_CPU == 8000000UL
	// 8M / 115200 == 69.444444 -> UCOS16 = 1, UCBR0 = 4, UCBRF0 = 5, UCBRS0 = 0x55 ("0.4378")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = UCOS16 | (5<<4) | 0x5500;
	UCA1BR0 = 4;
#elif F_CPU == 4000000UL
	// 4M / 115200 == 34.722222 -> UCOS16 = 1, UCBR0 = 2, UCBRF0 = 2, UCBRS0 = 0xbb ("0.7147")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = UCOS16 | (2<<4) | 0xbb00;
	UCA1BR0 = 2;
#elif F_CPU == 1000000UL
	// 1M / 115200 == 8.6805556 -> UCOS16 = 0, UCBR0 = 8, UCBRF0 = 0, UCBRS0 = 0xd6 ("0.6667")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = 0x5500;
	UCA1BR0 = 8;
#else
#error Unsupported F_CPU
#endif

	UCA1IRCTL = 0;
	UCA1ABCTL = 0;

	P2SEL0 &= ~BIT5;
	P2SEL1 |= BIT5;
	P2DIR |= BIT5;

	UCA1CTLW0 &= ~UCSWRST;
}

void StandardOutput1::put(char c)
{
	while (!(UCA1IFG & UCTXIFG));
	UCA1TXBUF = c;

	if (c == '\n') {
		put('\r');
	}
}

StandardOutput1 kout1;
