/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for MAX44006 RGB, IR, and Temperature Sensor.
 * Does not support interrupts.
 */
#include "driver/max44006.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

#include "arch.h"

uint8_t MAX44006::init()
{
	txbuf[0] = interruptStatusReg;
	if (i2c.xmit(address, 1, txbuf, 1, rxbuf) != 0) {
		return 1;
	}

	if (rxbuf[0] & ~PWRON) {
		// reset sensor
		txbuf[1] = 0x10;
		if (i2c.xmit(address, 2, txbuf, 0, rxbuf) != 0) {
			return 1;
		}
		arch.delay_ms(200);
		if (i2c.xmit(address, 1, txbuf, 1, rxbuf) != 0) {
			return 1;
		}
		if (rxbuf[0] & ~PWRON) {
			return 2;
		}
	}

	txbuf[0] = ambientConfigReg;
	txbuf[1] = TEMPEN; // TEMPEN = 1
	if (i2c.xmit(address, 2, txbuf, 0, rxbuf) != 0) {
		return 1;
	}

	txbuf[0] = mainConfigReg;
	txbuf[1] = MODE_10; // MODE = 10 -> Clear + RGB + IR measurement
	if (i2c.xmit(address, 2, txbuf, 0, rxbuf) != 0) {
		return 1;
	}

	return 0;
}

uint16_t MAX44006::getTemperature()
{
	txbuf[0] = 0x12;
	if (i2c.xmit(address, 1, txbuf, 2, rxbuf) != 0) {
		return 0;
	}

	return (((uint16_t)rxbuf[0] << 8) + rxbuf[1]);
}

bool MAX44006::getLight(float *red, float *green, float *blue, float *clear, float *ir)
{
	txbuf[0] = 0x04;
	if (i2c.xmit(address, 1, txbuf, 10, rxbuf) != 0) {
		return false;
	}

	*clear = (float)(((uint16_t)rxbuf[0] << 8) + rxbuf[1]) * 0.002;
	*red   = (float)(((uint16_t)rxbuf[2] << 8) + rxbuf[3]) * 0.002;
	*green = (float)(((uint16_t)rxbuf[4] << 8) + rxbuf[5]) * 0.002;
	*blue  = (float)(((uint16_t)rxbuf[6] << 8) + rxbuf[7]) * 0.004;
	*ir    = (float)(((uint16_t)rxbuf[8] << 8) + rxbuf[9]) * 0.002;

	return true;
}

MAX44006 max44006(0x45);
