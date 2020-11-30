#include "driver/stdout.h"
#include <msp430.h>

/*
 * Baud rate calculation according to datasheet:
 * N := f_{BRCLK} / Baudrate = F_CPU / 115200 in our case
 * if N <= 16: OS16 = 0, UCBR0 = int(N)
 * if N > 16: OS16 = 1, UCBR0 = int(N/16), UCBRF0 = int(((n/16) - int(n/16)) * 16) = int(N)%16
 * Set UCBRS0 according to table 21-4
 */

void StandardOutput::setup()
{
	UCA0CTLW0 |= UCSWRST;
#if F_CPU == 16000000UL
	// 16M / 115200 == 138.88889 -> UCOS16 = 1, UCBR0 = 16, UCBRF0 = 10, UCBRS0 = 0xf7 ("0.8751")
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = UCOS16 | (10<<4) | 0xF700;
	UCA0BR0 = 8;
#elif F_CPU == 8000000UL
	// 8M / 115200 == 69.444444 -> UCOS16 = 1, UCBR0 = 4, UCBRF0 = 5, UCBRS0 = 0x55 ("0.4378")
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = UCOS16 | (5<<4) | 0x5500;
	UCA0BR0 = 4;
#elif F_CPU == 4000000UL
	// 4M / 115200 == 34.722222 -> UCOS16 = 1, UCBR0 = 2, UCBRF0 = 2, UCBRS0 = 0xbb ("0.7147")
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = UCOS16 | (2<<4) | 0xbb00;
	UCA0BR0 = 2;
#elif F_CPU == 1000000UL
	// 1M / 115200 == 8.6805556 -> UCOS16 = 0, UCBR0 = 8, UCBRF0 = 0, UCBRS0 = 0xd6 ("0.6667")
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = 0x5500;
	UCA0BR0 = 8;
#elif F_CPU == 32768UL
	// Limited to 9600 Baud
	// 32768 / 9600 == 3.413333 -> UCOS16 = 0, UCBR0 = 3, UCBRF0 = 0, UCBRS0 = 0x92 ("0.4003")
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = 0x9200;
	UCA0BR0 = 3;
#else
#error Unsupported F_CPU
#endif

	UCA0IRCTL = 0;
	UCA0ABCTL = 0;

	P2SEL0 &= ~(BIT0 | BIT1);
	P2SEL1 |= BIT0 | BIT1;
	P2DIR |= BIT0;

	UCA0CTLW0 &= ~UCSWRST;

	//UCA0IE |= UCRXIE;
}

void StandardOutput::put(char c)
{
	while (!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = c;

	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
