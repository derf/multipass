#include "driver/stdout.h"
#include <msp430.h>

void StandardOutput::setup()
{
	UCA0CTLW0 |= UCSWRST;
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = UCOS16 | (10<<5) | 0xF700;
	UCA0BR0 = 8;

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
