#include "driver/mpu9250.h"
#include "arch.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include <stdint.h>

/*
Copyright (c) 2016 SparkFun Electronics, Inc.
Copyright (c) 2019 Daniel Friesel

SPDX-License-Identifier: MIT

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void MPU9250::init()
{
	// reset to default
	txbuf[0] = 107;
	txbuf[1] = 0x80;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	arch.delay_ms(100);

	// enable I2C passthrough to magnetometer
	txbuf[0] = 55;
	txbuf[1] = 1<<1;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);

	// reset magnetometer
	txbuf[0] = 0x0b;
	txbuf[1] = 1;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
	arch.delay_ms(100);

	// put magnetometer into continuous measurement mode
	txbuf[0] = 0x0a;
	txbuf[1] = 2;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
}

void MPU9250::setGyroEnable(bool x, bool y, bool z)
{
	txbuf[0] = 108;
	i2c.xmit(address, 1, txbuf, 1, txbuf + 1); // not a typo

	txbuf[1] &= ~0x07;

	if (!x) {
		txbuf[1] |= 4;
	}
	if (!y) {
		txbuf[1] |= 2;
	}
	if (!z) {
		txbuf[1] |= 1;
	}

	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void MPU9250::setAccelEnable(bool x, bool y, bool z)
{
	txbuf[0] = 108;
	i2c.xmit(address, 1, txbuf, 1, txbuf + 1); // not a typo

	txbuf[1] &= ~0x38;

	if (!x) {
		txbuf[1] |= 1<<5;
	}
	if (!y) {
		txbuf[1] |= 1<<4;
	}
	if (!z) {
		txbuf[1] |= 1<<3;
	}

	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void MPU9250::setGyroFSR(MPU9250::gyro_fsr_e fsr)
{
	txbuf[0] = MPU9250_GYRO_CONFIG;
	txbuf[1] = fsr << GYRO_CONFIG_GYRO_FS_SEL;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	gyro_fsr = fsr;
}

void MPU9250::setAccelFSR(MPU9250::accel_fsr_e fsr)
{
	txbuf[0] = MPU9250_ACCEL_CONFIG;
	txbuf[1] = fsr << ACCEL_CONFIG_ACCEL_FS_SEL;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	accel_fsr = fsr;
}

void MPU9250::AGSleep()
{
	txbuf[0] = 107;
	txbuf[1] = 1 << 6;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void MPU9250::MagSleep()
{
	txbuf[0] = 0x0a;
	txbuf[1] = 0;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
}

void MPU9250::AGWakeup()
{
	txbuf[0] = 107;
	txbuf[1] = gyroStandby << 4;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void MPU9250::MagWakeup()
{
	txbuf[0] = 0x0a;
	txbuf[1] = 2;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
}

void MPU9250::setGyroStandby(bool gyroStandby)
{
	this->gyroStandby = gyroStandby;
	txbuf[0] = 107;
	if (gyroStandby) {
		txbuf[1] = 1<<4;
	}
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

signed int MPU9250::getWordReg(unsigned char const regBase)
{
	txbuf[0] = regBase;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	return (int16_t)((rxbuf[0] << 8) + rxbuf[1]);
}

float MPU9250::getTemperature()
{
	return (float)getWordReg(65) / 333.87 + 21;
}

void MPU9250::getRawGyro(int *x, int *y, int *z)
{
	txbuf[0] = 67;
	i2c.xmit(address, 1, txbuf, 6, rxbuf);

	*x = (int16_t)(( rxbuf[0] << 8) + rxbuf[1]);
	*y = (int16_t)(( rxbuf[2] << 8) + rxbuf[3]);
	*z = (int16_t)(( rxbuf[4] << 8) + rxbuf[5]);
}

void MPU9250::getRawAccel(int *x, int *y, int *z)
{
	txbuf[0] = 59;
	i2c.xmit(address, 1, txbuf, 6, rxbuf);

	*x = (int16_t)(( rxbuf[0] << 8) + rxbuf[1]);
	*y = (int16_t)(( rxbuf[2] << 8) + rxbuf[3]);
	*z = (int16_t)(( rxbuf[4] << 8) + rxbuf[5]);
}

void MPU9250::getAccel(float *g_x, float *g_y, float *g_z)
{
	int x, y, z;
	unsigned char g_factor = 2;
	for (unsigned char i = 0; i < accel_fsr; i++) {
		g_factor *= 2;
	}

	getRawAccel(&x, &y, &z);

	float fsr_factor = g_factor / 32768.;

	*g_x = x * fsr_factor;
	*g_y = y * fsr_factor;
	*g_z = z * fsr_factor;
}

void MPU9250::getGyro(float *dps_x, float *dps_y, float *dps_z)
{
	int x, y, z;
	unsigned short dps_factor = 250;
	for (unsigned char i = 0; i < gyro_fsr; i++) {
		dps_factor *= 2;
	}

	getRawGyro(&x, &y, &z);

	float fsr_factor = dps_factor / 32768.;

	*dps_x = x * fsr_factor;
	*dps_y = y * fsr_factor;
	*dps_z = z * fsr_factor;
}

bool MPU9250::getRawMagnet(int *x, int *y, int *z)
{
	txbuf[0] = 0x02;
	i2c.xmit(0x0c, 1, txbuf, 8, rxbuf);

	if ((rxbuf[0] & 0x01) && !(rxbuf[7] & 0x08)) {
		*x = (int16_t)((rxbuf[2] << 8) + rxbuf[1]);
		*y = (int16_t)((rxbuf[4] << 8) + rxbuf[3]);
		*z = (int16_t)((rxbuf[6] << 8) + rxbuf[5]);
		return true;
	}
	else {
		return false;
	}
}

// mpu9250.dfa

void MPU9250::nineAxis()
{
	gyroStandby = false;
	AGWakeup();
	MagWakeup();
	setAccelEnable(true, true, true);
	setGyroEnable(true, true, true);
}

void MPU9250::sleep()
{
	AGSleep();
	MagSleep();
}

void MPU9250::standby()
{
	setGyroStandby(true);
	MagSleep();
	setAccelEnable(false, false, false);
	setGyroEnable(true, true, true);
}

void MPU9250::lowPowerAccelOnly(unsigned char rate)
{
	txbuf[0] = MPU9250_LP_ACCEL_ODR;
	txbuf[1] = rate & 0x0f;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	MagSleep();
	txbuf[0] = 107;
	txbuf[1] = 1<<5;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	setGyroEnable(false, false, false);
	setAccelEnable(true, true, true);
}

void MPU9250::accelOnly()
{
	gyroStandby = false;
	AGWakeup();
	MagSleep();
	setGyroEnable(false, false, false);
	setAccelEnable(true, true, true);
}

void MPU9250::gyroOnly()
{
	gyroStandby = false;
	AGWakeup();
	MagSleep();
	setAccelEnable(false, false, false);
	setGyroEnable(true, true, true);
}

void MPU9250::magnetOnly()
{
	gyroStandby = false;
	MagWakeup();
	AGSleep();
}

MPU9250 mpu9250;
