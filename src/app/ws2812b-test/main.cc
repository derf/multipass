/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/neopixel.h"
#include "driver/stdin.h"
#include "driver/stdout.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define NUM_PIXELS 256

Adafruit_NeoPixel np(NUM_PIXELS, GPIO::pc0, NEO_GRB+NEO_KHZ800);

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	np.setup();

	unsigned char pixel = 0;

	while (1) {
		gpio.led_toggle();
		np.setPixelColor(pixel, np.Color(0, 0, 0));
		if (pixel == 255) {
			pixel = 0;
		} else {
			pixel++;
		}
		np.setPixelColor(pixel, np.Color(64, 64, 64));
		np.show();
		_delay_ms(10);
	}

	return 0;
}
