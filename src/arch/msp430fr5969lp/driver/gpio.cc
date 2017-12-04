#include "driver/gpio.h"
#include <msp430.h>

void GPIO::setup()
{
	P1OUT = 0;
	P2OUT = 0;
	P3OUT = 0;
	P4OUT = 0;
	P1DIR = BIT0;
	P2DIR = 0;
	P3DIR = 0;
	P4DIR = BIT6;
}

void GPIO::led_on(unsigned char id)
{
	if (id == 0) {
		P1OUT |= BIT0;
	} else {
		P4OUT |= BIT6;
	}
}

void GPIO::led_off(unsigned char id)
{
	if (id == 0) {
		P1OUT &= ~BIT0;
	} else {
		P4OUT &= ~BIT6;
	}
}

void GPIO::led_toggle(unsigned char id)
{
	if (id == 0) {
		P1OUT ^= BIT0;
	} else {
		P4OUT ^= BIT6;
	}
}

GPIO gpio;
