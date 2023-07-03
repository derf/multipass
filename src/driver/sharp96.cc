/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for Sharp LS013B4DN04 transflective LCD, also available as
 * "430BOOST-SHARP96" addon for the MSP430 LaunchPad series.
 */
#include "driver/sharp96.h"
#include "driver/spi.h"
#include "driver/gpio.h"

#ifndef SHARP96_POWER_PIN
#error makeflag sharp96_power_pin required
#endif

#ifndef SHARP96_EN_PIN
#error makeflag sharp96_en_pin required
#endif

#ifndef SHARP96_CS_PIN
#error makeflag sharp96_cs_pin required
#endif

#define CMD_WRITE_LINE 0x80
#define CMD_TOGGLE_VCOM 0x40
#define CMD_CLEAR_SCREEN 0x20

void Sharp96::setup()
{
	gpio.output(SHARP96_POWER_PIN);
	gpio.output(SHARP96_EN_PIN);
	gpio.output(SHARP96_CS_PIN);
}

void Sharp96::powerOn()
{
	gpio.write(SHARP96_POWER_PIN, 1);
	gpio.write(SHARP96_EN_PIN, 1);
}

void Sharp96::powerOff()
{
	gpio.write(SHARP96_EN_PIN, 0);
	gpio.write(SHARP96_POWER_PIN, 0);
}

unsigned char Sharp96::swap_bits(unsigned char byte)
{
	byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
	byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
	byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
	return byte;
}

void Sharp96::clear()
{
	gpio.write(SHARP96_CS_PIN, 1);
	txbuf[0] = CMD_CLEAR_SCREEN | (CMD_TOGGLE_VCOM * vcom);
	txbuf[1] = 0;
	spi.xmit(2, txbuf, 0, txbuf);
	gpio.write(SHARP96_CS_PIN, 0);
}

void Sharp96::writeLine(unsigned char line_no, unsigned char* data)
{
	txbuf[0] = CMD_WRITE_LINE | (CMD_TOGGLE_VCOM * vcom);
	txbuf[1] = swap_bits(line_no + 1);
	gpio.write(SHARP96_CS_PIN, 1);
	spi.xmit(2, txbuf, 0, txbuf);
	spi.xmit(12, data, 0, txbuf);
	txbuf[0] = 0;
	txbuf[1] = 0;
	spi.xmit(2, txbuf, 0, txbuf);
	gpio.write(SHARP96_CS_PIN, 0);
}

void Sharp96::toggleVCOM()
{
	vcom = !vcom;
	txbuf[0] = CMD_TOGGLE_VCOM * vcom;
	txbuf[1] = 0;
	gpio.write(SHARP96_CS_PIN, 1);
	spi.xmit(2, txbuf, 0, txbuf);
	gpio.write(SHARP96_CS_PIN, 0);
}

Sharp96 sharp96;
