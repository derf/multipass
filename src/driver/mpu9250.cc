#include "driver/mpu9250.h"
#include "arch.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

void MPU9250::init()
{
	// reset to default
	txbuf[0] = 107;
	txbuf[1] = 0x80;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
	arch.delay_ms(1);

	// enable I2C passthrough to magnetometer
	txbuf[0] = 55;
	txbuf[1] = 1<<1;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);

	// reset magnetometer
	txbuf[0] = 0x0b;
	txbuf[1] = 1;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
	arch.delay_ms(1);

	// put magnetometer into continuous measurement mode
	txbuf[0] = 0x0a;
	txbuf[1] = 2;
	i2c.xmit(0x0c, 2, txbuf, 0, rxbuf);
}

void MPU9250::setGyroEnable(bool x, bool y, bool z)
{
	txbuf[0] = 108;
	i2c.xmit(address, 1, txbuf, 1, txbuf + 1); // not a typo

	txbuf[1] &= 0x07;

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

	txbuf[1] &= 0xf8;

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

void MPU9250::setPower(bool sleep, bool gyroStandby)
{
	txbuf[0] = 107;
	if (sleep) {
		txbuf[1] = 1<<6;
	}
	else if (gyroStandby) {
		txbuf[1] = 1<<4;
	}
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

signed int MPU9250::getWordReg(unsigned char const regBase)
{
	txbuf[0] = regBase;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	return ((signed int)rxbuf[0] << 8) + rxbuf[1];
}

float MPU9250::getTemperature()
{
	return (float)getWordReg(65) / 333.87 + 21;
}

void MPU9250::getMagnet(int *x, int *y, int *z)
{
	txbuf[0] = 0x02;
	i2c.xmit(0x0c, 0, txbuf, 8, rxbuf);

	if ((rxbuf[0] & 0x01) && !(rxbuf[7] & 0x08)) {
		*x = ((signed int)rxbuf[2] << 8) + rxbuf[1];
		*y = ((signed int)rxbuf[4] << 8) + rxbuf[3];
		*z = ((signed int)rxbuf[6] << 8) + rxbuf[5];
	}
	else {
		*x = *y = *z = 0;
	}
}

MPU9250 mpu9250;
