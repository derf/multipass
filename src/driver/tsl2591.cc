/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/tsl2591.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void TSL2591::init()
{
	txbuf[0] = 0xa0;
	txbuf[1] = 0x03;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	txbuf[0] = 0xa1;
	txbuf[1] = 0x01; // 200ms integration time
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void TSL2591::read()
{
	txbuf[0] = 0xb4;
	i2c.xmit(address, 1, txbuf, 4, rxbuf);
	ch0 = (rxbuf[1] << 8) + rxbuf[0];
	ch1 = (rxbuf[3] << 8) + rxbuf[2];
}

unsigned char TSL2591::getStatus()
{
	txbuf[0] = 0xb3;
	i2c.xmit(address, 1, txbuf, 1, rxbuf);
	return rxbuf[0];
}

TSL2591 tsl2591;
