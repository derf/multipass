/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/hdc1080.h"
#include "arch.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void HDC1080::measure()
{
	txbuf[0] = 0x00;
	i2c.xmit(address, 1, txbuf, 0, rxbuf);
}

float HDC1080::getTemp()
{
	txbuf[0] = 0x00;
	i2c.xmit(address, 0, txbuf, 2, rxbuf);

	return (((unsigned int)rxbuf[0] << 8) | rxbuf[1]) * .00251770019531250000 - 40.;
}

float HDC1080::getRH()
{
	txbuf[0] = 0x01;
	i2c.xmit(address, 0, txbuf, 2, rxbuf);

	return (((unsigned int)rxbuf[0] << 8) | rxbuf[1]) * .00152587890625000000;
}

unsigned int HDC1080::getManufacturerID()
{
	txbuf[0] = 0xfe;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);
	return (unsigned int)rxbuf[0] << 8 | rxbuf[1];
}

void HDC1080::heater(bool on)
{
	txbuf[0] = 0x02;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);
	if (on) {
		txbuf[1] = rxbuf[0] | (1<<5);
	} else {
		txbuf[1] = rxbuf[0] & ~(1<<5);
	}
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void HDC1080::init()
{
	txbuf[0] = 0x02;
	txbuf[1] = 0x00;
	txbuf[2] = 0x00;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
	arch.delay_ms(15);
}

HDC1080 hdc1080;
