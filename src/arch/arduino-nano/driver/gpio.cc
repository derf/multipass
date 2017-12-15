#include "driver/gpio.h"
#include <avr/io.h>

void GPIO::setup()
{
	DDRB = _BV(PB5);
}

void GPIO::led_on(unsigned char id)
{
	PORTB |= _BV(PB5);
}

void GPIO::led_off(unsigned char id)
{
	PORTB &= ~_BV(PB5);
}

void GPIO::led_toggle(unsigned char id)
{
	PINB = _BV(PB5);
}

GPIO gpio;
