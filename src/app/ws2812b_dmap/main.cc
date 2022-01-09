/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/neopixel.h"
#include "driver/stdout.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define CLOCK_LO ( ( PIND & _BV(PD3) ) != 0 )
#define CLOCK_HI ( ( PIND & _BV(PD3) ) == 0 )
#define DATA_LO ( ( PIND & _BV(PD2) ) != 0 )
#define DATA_HI ( ( PIND & _BV(PD2) ) == 0 )
#define DATA_BIT ( ( ~PIND & _BV(PD2) ) >> PD2 )

#define BUF_SIZE 36
#define BUF_MAX ((BUF_SIZE)-1)

#define NUM_PIXELS 256

Adafruit_NeoPixel np(NUM_PIXELS, GPIO::pb0, NEO_GRB+NEO_KHZ800);

#define MYADDRESS (0x0006)

volatile uint16_t address;
volatile uint8_t buf[BUF_SIZE];
volatile uint8_t done;

void update()
{
	if (buf[35] > 100) {
		buf[35] = 100;
	}
	if (buf[32] + buf[33] + buf[34] > 50) {
		buf[32] = 0;
		buf[33] = 0;
		buf[34] = 5;
	}
	uint32_t color = np.Color(buf[34], buf[33], buf[32]);
	uint8_t rgb = buf[35];
	for (uint8_t col = 0; col < 32; col++) {
		for (uint8_t row = 0; row < 8; row++) {
			uint8_t pixel_index = col*8;
			if (col % 2) {
				pixel_index += 7 - row;
			} else {
				pixel_index += row;
			}
			if (rgb) {
				color = np.gamma32(np.ColorHSV(((uint16_t)col*8+row) * 255, 255, rgb));
			}
			if (buf[col] & _BV(row)) {
				np.setPixelColor(pixel_index, color);
			} else {
				np.setPixelColor(pixel_index, 0);
			}
		}
	}
	np.show();
}

int main(void)
{

	arch.setup();
	gpio.setup();
	kout.setup();

	np.setup();

	PORTD = _BV(PD2) | _BV(PD3);
	EICRA = _BV(ISC10);
	EIMSK = _BV(INT1);

	np.setPixelColor(0, np.Color(0, 1, 1));
	np.setPixelColor(7, np.Color(0, 1, 1));
	np.setPixelColor(248, np.Color(0, 1, 1));
	np.setPixelColor(255, np.Color(0, 1, 1));
	np.show();

	while (1) {
		arch.idle();
		if (done) {
			update();
			done = 0;
		}
	}

	return 0;
}

ISR(INT1_vect)
{
	if (CLOCK_HI) {
		for (uint8_t i = BUF_MAX; i > 0; i--) {
			buf[i] = (buf[i] << 1) | (buf[i-1] >> 7);
		}
		buf[0] = (buf[0] << 1) | (address >> 15);
		address = (address << 1) | DATA_BIT;
		if (DATA_BIT) {
			gpio.led_on(0);
		} else {
			gpio.led_off(0);
		}
	}
	else if (DATA_HI && (address == MYADDRESS)) {
		done = 1;
	}
}
