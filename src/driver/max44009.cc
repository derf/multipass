/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/max44009.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

float MAX44009::getLux()
{
	unsigned char luxHigh;
	unsigned char luxLow;
	unsigned int mantissa, exponent;

	txbuf[0] = 0x03;
	txbuf[1] = 0x04;

	i2c.xmit(address, 2, txbuf, 2, rxbuf);

	luxHigh = rxbuf[0];
	luxLow = rxbuf[1];

	/*
	* The lowest 4 bit of luxLow are the lowest 4 bit of the mantissa.
	* The lowest 4 bit of luxHigh are the highest 4 bit of the mantissa.
	*/
	mantissa = (luxLow & 0x0F) + ((luxHigh & 0x0F) << 4);

	/*
	* The highest 4 bit of luxHigh are the 4 bit exponent
	*/
	exponent = (luxHigh & 0xF0) >> 4;

	/*
	* Cast base and mantissa to float to avoid calculation errors
	* because of 16bit integer overflows.
	*/
	return (float)(1 << exponent) * (float)mantissa * 0.045;
}

MAX44009 max44009(0x4a);
