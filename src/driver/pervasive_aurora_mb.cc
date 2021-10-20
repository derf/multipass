/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for Pervasive Aurora Mb E-Paper displays with internal Timing
 * Controller (iTC). Configured for the 300x400px 4.2" variant.
 */
#include "driver/pervasive_aurora_mb.h"
#include "driver/spi.h"
#include "driver/gpio.h"
#include "arch.h"

void PervasiveAuroraMb::setup()
{
	gpio.output(PERVASIVE_AURORA_RESET_PIN, 0);
	gpio.output(PERVASIVE_AURORA_CS_PIN, 0);
	gpio.output(PERVASIVE_AURORA_DC_PIN, 0);
	gpio.output(PERVASIVE_AURORA_VCC_PIN, 0);
	gpio.input(PERVASIVE_AURORA_BUSY_PIN);
}

void PervasiveAuroraMb::powerOn()
{
	gpio.write(PERVASIVE_AURORA_VCC_PIN, 1);
	arch.delay_ms(5);
	gpio.write(PERVASIVE_AURORA_RESET_PIN, 1);
	arch.delay_ms(1);
	gpio.write(PERVASIVE_AURORA_CS_PIN, 1);
}

void PervasiveAuroraMb::initialize(signed char temperature)
{
	// "Input Temperature"
	spiWrite(0xe5, (const unsigned char*)&temperature, 1);

	// "Active Temperature"
	spiWrite(0xe0, (const unsigned char[]){0x02}, 1);

	// "Panel Settings"
	spiWrite(0x00, (const unsigned char[]){0x0f}, 1);

	// "Panel Settings"
	spiWrite(0x00, (const unsigned char[]){0x0f}, 1);

	// "Booster soft start settings"
	spiWrite(0x06, (const unsigned char[]){0x17, 0x17, 0x27}, 3);

	// "Resolution settings": 300 x 400 -> 0x12c x 0x190
	spiWrite(0x61, (const unsigned char[]){0x01, 0x90, 0x01, 0x2c}, 4);

	// "Vcom and data interval setting"
	spiWrite(0x50, (const unsigned char[]){0x87}, 1);

	// "Power Saving"
	spiWrite(0xe3, (const unsigned char[]){0x88}, 1);
}

void PervasiveAuroraMb::sendImage(unsigned char *frame, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	unsigned char null_int[2] = {0x00, 0x00};
	spiWrite(0x10, 0, 0);

	gpio.write(PERVASIVE_AURORA_CS_PIN, 0);
	spi.xmit(300*(400/8), frame, 0, frame);
	gpio.write(PERVASIVE_AURORA_CS_PIN, 1);

	/*
	 * Second Pass: All zeroes (-> update entire display).
	 * A 1 bit indicates that the corresponding pixel should be skipped.
	 */
	spiWrite(0x13, 0, 0);
	gpio.write(PERVASIVE_AURORA_CS_PIN, 0);
	if (w == 0 || h == 0) {
		for (unsigned int i = 0; i < 300*(400/16); i++) {
			spi.xmit(2, null_int, 0, frame);
		}
	} else {
		unsigned int y1_mod = y % 8;
		unsigned int y2_mod = (y + h) % 8;
		for (unsigned int x_pos = 0; x_pos < 300; x_pos++) {
			for (unsigned int y_pos = 0; y_pos < 400/8; y_pos++) {
				if (x_pos < x || x_pos >= x+w || y_pos < y/8 || y_pos > (y+h)/8) {
					// outside of update area
					null_int[0] = 0xff;
				} else  if (y/8 == (y+h)/8) {
					null_int[0] = (0xff << y2_mod) | (0xff >> (8-y1_mod));
				} else if (y_pos == y/8) {
					null_int[0] = 0xff << y1_mod;
				} else if (y_pos == (y+h)/8) {
					null_int[0] = 0xff >> (8-y2_mod);
				} else {
					null_int[0] = 0x00;
				}
				spi.xmit(1, null_int, 0, frame);
			}
		}
	}
	gpio.write(PERVASIVE_AURORA_CS_PIN, 1);
}

void PervasiveAuroraMb::sendUpdate()
{
	while (isBusy()) ;

	spiWrite(0x04, 0, 0);

	while (isBusy()) ;

	spiWrite(0x12, 0, 0);

	while (isBusy()) ;
}

void PervasiveAuroraMb::powerOff()
{
	spiWrite(0x02, 0, 0);
	while (isBusy()) ;
	gpio.output(PERVASIVE_AURORA_RESET_PIN, 0);
	gpio.output(PERVASIVE_AURORA_CS_PIN, 0);
	gpio.output(PERVASIVE_AURORA_DC_PIN, 0);
	gpio.output(PERVASIVE_AURORA_VCC_PIN, 0);
}

bool PervasiveAuroraMb::isBusy()
{
	return !gpio.read(PERVASIVE_AURORA_BUSY_PIN);
}

void PervasiveAuroraMb::spiWrite(unsigned char reg, const unsigned char *txbuf, unsigned int length)
{
	gpio.write(PERVASIVE_AURORA_DC_PIN, 0);
	gpio.write(PERVASIVE_AURORA_CS_PIN, 0);

	spi.xmit(1, &reg, 0, 0);

	gpio.write(PERVASIVE_AURORA_CS_PIN, 1);
	gpio.write(PERVASIVE_AURORA_DC_PIN, 1);

	if (length) {
		arch.delay_us(1);
		gpio.write(PERVASIVE_AURORA_CS_PIN, 0);
		spi.xmit(length, (unsigned char*)txbuf, 0, 0);
		gpio.write(PERVASIVE_AURORA_CS_PIN, 1);
	}
}

PervasiveAuroraMb pervasiveAuroraMb;
