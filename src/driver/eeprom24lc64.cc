/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <stdlib.h>
#include "driver/eeprom24lc64.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

void EEPROM24LC64::readPage(unsigned short addr, char *data)
{
	txbuf[0] = addr >> 8;
	txbuf[1] = addr & 0x00ff;
	i2c.xmit(address, 2, txbuf, 32, (unsigned char *)data);
}

void EEPROM24LC64::writePage(unsigned short addr, char *data)
{
	txbuf[0] = addr >> 8;
	txbuf[1] = addr & 0x00ff;
	for (unsigned char i = 0; i < 32; i++) {
		txbuf[i+2] = data[i];
	}
	i2c.xmit(address, 34, txbuf, 0, NULL);
}

EEPROM24LC64 eeprom24lc64(0x50);
