/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/spi_a1.h"
#include <msp430.h>

signed char SPI::setup()
{
	/* UCA1CLK Pin 2.4 */
	P2SEL0 &= ~BIT4;
	P2SEL1 |= BIT4;
	P2DIR |= BIT4;

	/* UCA1SIMO Pin 2.5 */
	P2SEL0 &= ~BIT5;
	P2SEL1 |= BIT5;
	P2DIR |= BIT5;

	/* UCA1SOMI Pin 2.6 */
	P2SEL0 &= ~BIT6;
	P2SEL1 |= BIT6;
	P2DIR &= ~BIT6;
	//P2REN |= BIT6;

	UCA1CTLW0 |= UCSWRST;
	UCA1MCTLW = 0; // no modulation

	UCA1CTLW0 = UCCKPH | UCMSB | UCMST | UCSYNC | UCMODE_0 | UCSSEL__SMCLK | UCSWRST;
	UCA1BRW = 15; // /16 -> 1MHz
	UCA1CTLW0 &= ~UCSWRST;
}

signed char SPI::xmit(unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	volatile char rxbuf_cache;
	if (tx_len < 1) {
		return -1;
	}

	UCA1IE &= ~(UCTXIE | UCRXIE);

	while (UCA1STATW & UCBUSY) ;

	rxbuf_cache = UCA1RXBUF;
	UCA1TXBUF = tx_buf[0];

	unsigned char tx_pos = 1;
	unsigned char rx_pos = 0;

	while (tx_pos < tx_len || rx_pos < rx_len) {
		if ((tx_pos < tx_len) && (UCA1IF & UCTXIFG)) {
			UCBA1TXBUF = tx_buf[tx_pos++];
		}
		if (UCA1IFG & UCRXIFG) {
			if (rx_pos < rx_len) {
				rx_buf[rx_pos] = UCA1RXBUF;
			} else {
				rxbuf_cache = UCA1RXBUF;
			}
		}
		rx_pos++;
	}
}
