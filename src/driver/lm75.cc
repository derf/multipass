/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for LM75B Digital Temperature Sensor and Thermal Watchdog.
 */
#include "driver/lm75.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

#include <stdint.h>

float LM75::getTemp()
{
	txbuf[0] = 0;
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	return (int8_t)rxbuf[0] + (rxbuf[1] / 256.0);
}

unsigned int LM75::getOS()
{
	txbuf[0] = 0x03;
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);
	return rxbuf[0];
}

unsigned int LM75::getHyst()
{
	txbuf[0] = 0x02;
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);
	return rxbuf[0];
}

void LM75::setOS(signed char os)
{
	txbuf[0] = 0x03;
	txbuf[1] = os;
	txbuf[2] = 0;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
}

void LM75::setHyst(signed char hyst)
{
	txbuf[0] = 0x02;
	txbuf[1] = hyst;
	txbuf[2] = 0;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
}

void LM75::init()
{
	txbuf[0] = 0x01;
	txbuf[1] = 0x00;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void LM75::shutdown()
{
	txbuf[0] = 0x01;
	txbuf[1] = 0x01;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

LM75 lm75(0x48);
