#include "driver/spi.h"
#include <msp430.h>

#ifndef F_I2C
#define F_I2C 1000000UL
#endif

void SPI::setup()
{
	UCB0CTLW0 |= UCSWRST;

	/* UCB0CLK Pin 2.2 */
	P2SEL0 &= ~BIT2;
	P2SEL1 |= BIT2;
	P2DIR |= BIT2;

	/* UCB0SIMO Pin 1.6 */
	P1SEL0 &= ~BIT6;
	P1SEL1 |= BIT6;
	P1DIR |= BIT6;

	/* UCB0SOMI Pin 1.7 */
	P1SEL0 &= ~BIT7;
	P1SEL1 |= BIT7;
	P1DIR &= ~BIT7;
	//P1REN |= BIT6;

	UCB0CTLW0 = UCCKPH | UCMSB | UCMST | UCSYNC | UCMODE_0 | UCSSEL__SMCLK | UCSWRST;
	UCB0BRW = (F_CPU/F_I2C)-1; // /16 -> 1MHz
	// UCB0BRW = (F_CPU / F_I2C) - 1
	UCB0CTLW0 &= ~UCSWRST;
}

static inline unsigned char clean_rxbuf()
{
	return UCB0RXBUF;
}

signed char SPI::xmit(unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	if (tx_len < 1) {
		return -1;
	}

	while (UCB0STATW & UCBUSY) ;

	if (!(UCB0IFG & UCTXIFG)) {
		return -1;
	}

	UCB0IFG &= ~UCRXIFG;
	UCB0TXBUF = tx_buf[0];

	unsigned char tx_pos = 1;
	unsigned char rx_pos = 0;

	while (tx_pos < tx_len || rx_pos < rx_len) {
		if ((tx_pos < tx_len) && (UCB0IFG & UCTXIFG)) {
			UCB0TXBUF = tx_buf[tx_pos++];
		}
		if (UCB0IFG & UCRXIFG) {
			if (rx_pos < rx_len) {
				rx_buf[rx_pos] = UCB0RXBUF;
			} else {
				UCB0IFG &= ~UCRXIFG;
			}
			rx_pos++;
		}
	}
	while (UCB0STATW & UCBUSY) ;
	return 0;
}

SPI spi;
