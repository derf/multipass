/*
 * Copyright 2020 Birte Kristina Friesel
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
#define RINGBUF_SIZE ((BUF_SIZE)+2)

#define NUM_PIXELS 256

Adafruit_NeoPixel np(NUM_PIXELS, GPIO::pb0, NEO_GRB+NEO_KHZ800);

#define MYADDRESS (0x0006)

volatile uint16_t address;
volatile uint8_t ringbuf[RINGBUF_SIZE];
volatile uint8_t ringbuf_byte = 0;
volatile uint8_t ringbuf_bitmask = 0x80;
volatile uint8_t buf[BUF_SIZE];
volatile uint8_t done;

void update()
{
	if (buf[0] > 100) {
		buf[0] = 100;
	}
	if (buf[1] + buf[2] + buf[3] > 50) {
		buf[1] = 5;
		buf[2] = 0;
		buf[3] = 0;
	}
	uint32_t color = np.Color(buf[1], buf[2], buf[3]);
	uint8_t rgb = buf[0];
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
			if (buf[31-col+4] & _BV(row)) {
				np.setPixelColor(pixel_index, color);
			} else {
				np.setPixelColor(pixel_index, 0);
			}
		}
	}
	np.show();
}

void next_bit(uint8_t *byte, uint8_t *mask, uint8_t const size)
{
	if (*mask > 0x01) {
		*mask >>= 1;
	} else {
		*byte = (*byte + 1) % size;
		*mask = 0x80;
	}
}

void ringbuf_to_buf(void)
{
	uint8_t read_byte = ringbuf_byte;
	uint8_t read_bitmask = ringbuf_bitmask;

	// byte[bitmask] is the next bit that would be written -- and, thanks to
	// the buffer size, also the most significant bit of message byte 0.

	for (uint8_t write_byte = 0; write_byte < BUF_SIZE; write_byte++) {
		buf[write_byte] = 0;
		for (uint8_t write_bitmask = 0x80; write_bitmask > 0; write_bitmask >>= 1) {
			if (ringbuf[read_byte] & read_bitmask) {
				buf[write_byte] |= write_bitmask;
			}
			if (read_bitmask > 0x01) {
				read_bitmask >>= 1;
			} else {
				read_byte = (read_byte + 1) % RINGBUF_SIZE;
				read_bitmask = 0x80;
			}
		}
	}
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
			/*
			kout << dec << "done, byte=" << ringbuf_byte << hex << " bitmask=" << ringbuf_bitmask << "  ringbuf = " << hex;
			for (uint8_t i = 0; i < RINGBUF_SIZE; i++) {
				kout << ringbuf[i] << " ";
			}
			kout << endl;
			*/
			ringbuf_to_buf();
			/*
			kout << "update, buf = " << hex;
			for (uint8_t i = 0; i < BUF_SIZE; i++) {
				kout << buf[i] << " ";
			}
			kout << endl;
			*/
			update();
			done = 0;
		}
	}

	return 0;
}

ISR(INT1_vect)
{
	if (CLOCK_HI && !done) {
		if (DATA_BIT) {
			ringbuf[ringbuf_byte] |= ringbuf_bitmask;
			address = (address << 1) | 1;
			gpio.led_on(0);
		} else {
			ringbuf[ringbuf_byte] &= ~ringbuf_bitmask;
			address = (address << 1) | 0;
			gpio.led_off(0);
		}
		if (ringbuf_bitmask > 0x01) {
			ringbuf_bitmask >>= 1;
		} else {
			ringbuf_byte = (ringbuf_byte + 1) % RINGBUF_SIZE;
			ringbuf_bitmask = 0x80;
		}
	}
	else if (DATA_HI && (address == MYADDRESS)) {
		done = 1;
	}
}
