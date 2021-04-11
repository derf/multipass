/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/ssd1306.h"
#include "driver/i2c.h"
#include "driver/soft_i2c.h"

void SSD1306::writeCommand(uint8_t command)
{
	txbuf[0] = 0x80;
	txbuf[1] = command;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SSD1306::init()
{
	unsigned char i;
	for (i = 0; i < sizeof(init1); i++) {
		writeCommand(init1[i]);
	}
	writeCommand(height - 1);
	for (i = 0; i < sizeof(init2); i++) {
		writeCommand(init2[i]);
	}
	writeCommand(height == 32 ? 0x02 : 0x12);
	for (i = 0; i < sizeof(init3); i++) {
		writeCommand(init3[i]);
	}
}

void SSD1306::setContrast(unsigned char contrast)
{
	writeCommand(SSD1306_SET_CONTRAST);
	writeCommand(contrast);
}

void SSD1306::setInvert(bool invert)
{
	writeCommand(SSD1306_SET_NORM_INV | invert);
}

void SSD1306::showImage(unsigned char* data, uint16_t length)
{
	uint8_t i;
	uint8_t j;
	txbuf[0] = 0x40;
	for (i = 0; i < length / 128; i++) {
		for (j = 0; j < 128; j++) {
			txbuf[j+1] = data[(uint16_t)i*128 + j];
		}
		i2c.xmit(address, 129, txbuf, 0, rxbuf);
	}
	if (length % 128) {
		for (j = 0; j < length % 128; j++) {
			txbuf[j+1] = data[(uint16_t)i*128 + j];
		}
		i2c.xmit(address, (length % 128) + 1, txbuf, 0, rxbuf);
	}
}

SSD1306 ssd1306;
