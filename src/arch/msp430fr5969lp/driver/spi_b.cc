#include "driver/spi.h"
#include <msp430.h>

signed char SPI::setup()
{
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
	UCB0BRW = 15; // /16 -> 1MHz
	UCB0CTLW0 &= ~UCSWRST;
}

signed char SPI::xmit(unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	volatile char rxbuf_cache;
	if (tx_len < 1) {
		return -1;
	}

	UCB0IE &= ~(UCTXIE | UCRXIE);

	while (UCB0STATW & UCBUSY) ;

	rxbuf_cache = UCB0RXBUF;
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
				rxbuf_cache = UCB0RXBUF;
			}
			rx_pos++;
		}
	}
}
