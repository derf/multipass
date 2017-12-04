#include "driver/gpio.h"
#include <stdio.h>

void GPIO::led_on(unsigned char id)
{
	if (id < 8) {
		printf("▶ LED %3d on\n", id);
		ledstate |= (1 << id);
	}
}

void GPIO::led_off(unsigned char id)
{
	if (id < 8) {
		printf("▶ LED %3d off\n", id);
		ledstate &= ~(1 << id);
	}
}

void GPIO::led_toggle(unsigned char id)
{
	if (id < 8) {
		if (ledstate & (1 << id)) {
			led_off(id);
		} else {
			led_on(id);
		}
	}
}

GPIO gpio;
