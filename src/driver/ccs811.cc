/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/ccs811.h"
#include "driver/gpio.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

#ifdef MULTIPASS_ARCH_esp8266
#define nWAKE GPIO::d5
#endif

void CCS811::init()
{
	gpio.output(nWAKE);
	gpio.write(nWAKE, 1);
}

short CCS811::check()
{
	gpio.write(nWAKE, 0);
	txbuf[0] = 0x20;
	rxbuf[0] = 0;
	i2c.xmit(address, 1, txbuf, 1, rxbuf);
	gpio.write(nWAKE, 1);
	return rxbuf[0];
}

CCS811 ccs811(0x5a);
