/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/spi.h"
#include <msp430.h>

#ifndef F_I2C
#define F_I2C 1000000UL
#endif

void SPI::setup()
{
	UCB1CTLW0 |= UCSWRST;

	/* UCB1CLK Pin 5.2 */
	P5SEL1 &= ~BIT2;
	P5SEL0 |= BIT2;
	P5DIR |= BIT2;

	/* UCB1SIMO Pin 5.0 */
	P5SEL1 &= ~BIT0;
	P5SEL0 |= BIT0;
	P5DIR |= BIT0;

	/* UCB1SOMI Pin 5.1 */
	P5SEL1 &= ~BIT1;
	P5SEL0 |= BIT1;
	P5DIR &= ~BIT1;
	//P1REN |= BIT6;

	UCB1CTLW0 = UCCKPH | UCMSB | UCMST | UCSYNC | UCMODE_0 | UCSSEL__SMCLK | UCSWRST;
	UCB1BRW = (F_CPU/F_I2C)-1; // /16 -> 1MHz
	// UCB1BRW = (F_CPU / F_I2C) - 1
	UCB1CTLW0 &= ~UCSWRST;
}

static inline unsigned char clean_rxbuf()
{
	return UCB1RXBUF;
}

signed char SPI::xmit(unsigned int tx_len, unsigned char *tx_buf,
		unsigned int rx_len, unsigned char *rx_buf)
{
	if (tx_len < 1) {
		return -1;
	}

	while (UCB1STATW & UCBUSY) ;

	if (!(UCB1IFG & UCTXIFG)) {
		return -1;
	}

	UCB1IFG &= ~UCRXIFG;
	UCB1TXBUF = tx_buf[0];

	unsigned char tx_pos = 1;
	unsigned char rx_pos = 0;

	while (tx_pos < tx_len || rx_pos < rx_len) {
		if ((tx_pos < tx_len) && (UCB1IFG & UCTXIFG)) {
			UCB1TXBUF = tx_buf[tx_pos++];
		}
		if (UCB1IFG & UCRXIFG) {
			if (rx_pos < rx_len) {
				rx_buf[rx_pos] = UCB1RXBUF;
			} else {
				UCB1IFG &= ~UCRXIFG;
			}
			rx_pos++;
		}
	}
	while (UCB1STATW & UCBUSY) ;
	return 0;
}

SPI spi;
