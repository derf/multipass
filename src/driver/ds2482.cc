/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * DS2482-100 Single-Channel 1-Wire Master
 */
#include "driver/ds2482.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "arch.h"

void DS2482::setup()
{
	txbuf[0] = 0xf0;
	i2c.xmit(address, 1, txbuf, 0, rxbuf);
	txbuf[0] = 0xd2;
	txbuf[1] = 0xf0; // default setting: passive pull-up, standard speed
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void DS2482::busReset()
{
	txbuf[0] = 0xb4;
	i2c.xmit(address, 1, txbuf, 0, rxbuf);
}

unsigned char DS2482::status()
{
	txbuf[0] = 0xe1;
	txbuf[0] = 0xf0;
	i2c.xmit(address, 2, txbuf, 1, rxbuf);
	return rxbuf[0];
}

void DS2482::readROM(unsigned char *data, unsigned char len)
{
	busReset();
	arch.delay_ms(2); // reset low time (630us) + reset high time (614us)
	txbuf[0] = 0xa5;
	txbuf[1] = 0x33;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	arch.delay_us(800);
	for (unsigned char i = 0; i < len; i++) {
		txbuf[0] = 0x96;
		i2c.xmit(address, 1, txbuf, 0, rxbuf);
		arch.delay_us(800);
		txbuf[0] = 0xe1;
		txbuf[1] = 0xe1;
		i2c.xmit(address, 2, txbuf, 1, rxbuf);
		data[i] = rxbuf[0];
	}
}

DS2482 ds2482(0x18);
