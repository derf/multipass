/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/scd4x.h"
#include "driver/gpio.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void SCD4x::start()
{
	txbuf[0] = 0x21;
	txbuf[1] = 0xb1;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SCD4x::stop()
{
	txbuf[0] = 0x3f;
	txbuf[1] = 0x86;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SCD4x::startLowPower()
{
	txbuf[0] = 0x21;
	txbuf[1] = 0xac;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SCD4x::read()
{
	txbuf[0] = 0xec;
	txbuf[1] = 0x05;

	if (i2c.xmit(address, 1, txbuf, 9, rxbuf) == 0) {
		co2 = (rxbuf[0] << 8) + rxbuf[1];
		rawTemperature = ((rxbuf[3] << 8) + rxbuf[4]);
		rawHumidity = (rxbuf[6] << 8) + rxbuf[7];
	}
}

SCD4x scd4x;
