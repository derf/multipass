/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/i2c.h"
#include "arch.h"
#include <msp430.h>

#ifndef F_I2C
#define F_I2C 100000
#endif

volatile unsigned short old_ifg = 0;

#if (F_CPU / F_I2C) < 45
inline void await_i2c_int(unsigned int ie_flags, unsigned int ifg_flags) {
	while (!(UCB1IFG & ifg_flags)) ;
	if (UCB1IFG & (UCNACKIFG | UCCLTOIFG)) {
		UCB1IFG &= ~(UCNACKIFG | UCCLTOIFG);
	}
}
#else
inline void await_i2c_int(unsigned int ie_flags, unsigned int ifg_flags)
{
	UCB1IFG = 0;
	old_ifg = 0;
	UCB1IE = ie_flags;
	do {
		arch.idle();
	} while (!(old_ifg & ifg_flags));
	UCB1IE = 0;
}
#endif

signed char I2C::setup()
{
#ifdef I2C_PULLUP_FIXED_GPIO
	P8DIR |= BIT2 | BIT3;
	P8OUT |= BIT2 | BIT3;
#endif
	UCB1CTL1 = UCSWRST;
	UCB1CTLW0 = UCMODE_3 | UCMST | UCSYNC | UCSSEL_2 | UCSWRST | UCCLTO_1;
	UCB1BRW = (F_CPU / F_I2C) - 1;
	P5DIR &= ~(BIT0 | BIT1);
	P5SEL0 |= (BIT0 | BIT1);
	P5SEL1 &= ~(BIT0 | BIT1);

	UCB1CTL1 &= ~UCSWRST;
	UCB1I2CSA = 0;

	arch.delay_us(100);

	if (UCB1STAT & UCBBUSY)
		return -1;

	return 0;
}

void I2C::scan(unsigned int *results)
{
	for (unsigned char address = 0; address < 128; address++) {
		UCB1I2CSA = address;
		UCB1CTL1 |= UCTR | UCTXSTT | UCTXSTP;

		while (UCB1CTL1 & UCTXSTP);

		if (UCB1IFG & UCNACKIFG) {
			UCB1IFG &= ~UCNACKIFG;
		} else {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
		}
	}
	UCB1IFG = 0;
}

signed char I2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;
	UCB1I2CSA = address;
	if (tx_len) {
		UCB1CTL1 |= UCTR | UCTXSTT;
		for (i = 0; i < tx_len; i++) {
			await_i2c_int(UCTXIE0 | UCNACKIE | UCCLTOIE, UCTXIFG0 | UCNACKIFG | UCCLTOIFG);
			if (old_ifg & (UCNACKIFG | UCCLTOIFG)) {
				UCB1CTL1 |= UCTXSTP;
				return -1;
			}
			old_ifg = 0;
			UCB1TXBUF = tx_buf[i];
		}
		await_i2c_int(UCTXIE0 | UCNACKIE | UCCLTOIE, UCTXIFG0 | UCNACKIFG | UCCLTOIFG);
		//if (UCB1IFG & (UCNACKIFG | UCCLTOIFG)) {
		//	UCB1IFG &= ~UCNACKIFG;
		//	UCB1IFG &= ~UCCLTOIFG;
		//	UCB1CTL1 |= UCTXSTP;
		//	return -1;
		//}
	}
	if (rx_len) {
		UCB1I2CSA = address;
		UCB1IFG = 0;
		UCB1CTL1 &= ~UCTR;
		UCB1CTL1 |= UCTXSTT;

		while (UCB1CTL1 & UCTXSTT);
		UCB1IFG &= ~UCTXIFG0;

		for (i = 0; i < rx_len; i++) {
			if (i == rx_len - 1)
				UCB1CTL1 |= UCTXSTP;
			await_i2c_int(UCRXIE | UCNACKIE | UCCLTOIE, UCRXIFG0 | UCNACKIFG | UCCLTOIFG);
			rx_buf[i] = UCB1RXBUF;
			UCB1IFG &= ~UCRXIFG0;
		}
		UCB1IFG &= ~UCRXIFG0;
	}

	UCB1CTL1 |= UCTXSTP;

	while (UCB1CTL1 & UCTXSTP);
	return 0;
}

__attribute__((interrupt(USCI_B1_VECTOR))) __attribute__((wakeup)) void handle_usci_b1()
{
	old_ifg = UCB1IFG;
	UCB1IFG = 0;
}

I2C i2c;
