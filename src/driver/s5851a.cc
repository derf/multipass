/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/s5851a.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

float S5851A::getTemp()
{
	txbuf[0] = 0;
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	return rxbuf[0] + (rxbuf[1] / 256.0);
}

S5851A s5851a(0x4b);
