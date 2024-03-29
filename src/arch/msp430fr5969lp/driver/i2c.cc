/*
 * Copyright 2020 Birte Kristina Friesel
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
	while (!(UCB0IFG & ifg_flags)) ;
	if (UCB0IFG & (UCNACKIFG | UCCLTOIFG)) {
		UCB0IFG &= ~(UCNACKIFG | UCCLTOIFG);
	}
}
#else
inline void await_i2c_int(unsigned int ie_flags, unsigned int ifg_flags)
{
	UCB0IFG = 0;
	old_ifg = 0;
	UCB0IE = ie_flags;
	do {
		arch.idle();
	} while (!(old_ifg & ifg_flags));
	UCB0IE = 0;
}
#endif

signed char I2C::setup()
{
#ifdef CONFIG_I2C_PULLUP_FIXED_GPIO
	P1DIR |= BIT4 | BIT5;
	P1OUT |= BIT4 | BIT5;
#endif
	UCB0CTL1 = UCSWRST;
	UCB0CTLW0 = UCMODE_3 | UCMST | UCSYNC | UCSSEL_2 | UCSWRST | UCCLTO_1;
	UCB0BRW = (F_CPU / F_I2C) - 1;
	P1DIR &= ~(BIT6 | BIT7);
	P1SEL0 &= ~(BIT6 | BIT7);
	P1SEL1 |= BIT6 | BIT7;

	UCB0CTL1 &= ~UCSWRST;
	UCB0I2CSA = 0;

	arch.delay_us(100);

	if (UCB0STAT & UCBBUSY)
		return -1;

	return 0;
}

void I2C::scan(unsigned int *results)
{
	for (unsigned char address = 0; address < 128; address++) {
		UCB0I2CSA = address;
		UCB0CTL1 |= UCTR | UCTXSTT | UCTXSTP;

		while (UCB0CTL1 & UCTXSTP);

		if (UCB0IFG & UCNACKIFG) {
			UCB0IFG &= ~UCNACKIFG;
		} else {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
		}
	}
	UCB0IFG = 0;
}

signed char I2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;
	UCB0I2CSA = address;
	if (tx_len) {
		UCB0CTL1 |= UCTR | UCTXSTT;
		for (i = 0; i < tx_len; i++) {
			await_i2c_int(UCTXIE0 | UCNACKIE | UCCLTOIE, UCTXIFG0 | UCNACKIFG | UCCLTOIFG);
			if (old_ifg & (UCNACKIFG | UCCLTOIFG)) {
				UCB0CTL1 |= UCTXSTP;
				return -1;
			}
			old_ifg = 0;
			UCB0TXBUF = tx_buf[i];
		}
		await_i2c_int(UCTXIE0 | UCNACKIE | UCCLTOIE, UCTXIFG0 | UCNACKIFG | UCCLTOIFG);
		//if (UCB0IFG & (UCNACKIFG | UCCLTOIFG)) {
		//	UCB0IFG &= ~UCNACKIFG;
		//	UCB0IFG &= ~UCCLTOIFG;
		//	UCB0CTL1 |= UCTXSTP;
		//	return -1;
		//}
	}
	if (rx_len) {
		UCB0I2CSA = address;
		UCB0IFG = 0;
		UCB0CTL1 &= ~UCTR;
		UCB0CTL1 |= UCTXSTT;

		while (UCB0CTL1 & UCTXSTT);
		UCB0IFG &= ~UCTXIFG0;

		for (i = 0; i < rx_len; i++) {
			if (i == rx_len - 1)
				UCB0CTL1 |= UCTXSTP;
			await_i2c_int(UCRXIE | UCNACKIE | UCCLTOIE, UCRXIFG0 | UCNACKIFG | UCCLTOIFG);
			rx_buf[i] = UCB0RXBUF;
			UCB0IFG &= ~UCRXIFG0;
		}
		UCB0IFG &= ~UCRXIFG0;
	}

	UCB0CTL1 |= UCTXSTP;

	while (UCB0CTL1 & UCTXSTP);
	return 0;
}

__attribute__((interrupt(USCI_B0_VECTOR))) __attribute__((wakeup)) void handle_usci_b0()
{
	old_ifg = UCB0IFG;
	UCB0IFG = 0;
}

I2C i2c;
