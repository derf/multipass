/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/ccs811.h"
#include "driver/gpio.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void CCS811::init()
{
	startFirmware();
	//setMode(1);
	//setEnv(50, 0, 0, 0);
}

// switch to Application mode
void CCS811::startFirmware()
{
	if (getStatus() & 0x10) {
		txbuf[0] = 0xf4;
		i2c.xmit(address, 1, txbuf, 0, rxbuf);
	}
}

unsigned char CCS811::getManufacturerID()
{
	txbuf[0] = 0x20;
	i2c.xmit(address, 1, txbuf, 1, rxbuf);
	return rxbuf[0];
}

void CCS811::setMode(unsigned char mode)
{
	txbuf[0] = 0x01;
	txbuf[1] = (mode & 0x07) << 4;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void CCS811::read()
{
	txbuf[0] = 0x02;
	i2c.xmit(address, 1, txbuf, 8, rxbuf);
	eco2 = (rxbuf[0] << 8) + rxbuf[1];
	tvoc = (rxbuf[2] << 8) + rxbuf[3];
	status = rxbuf[4];
	error_id = rxbuf[5];
}

void CCS811::setEnv(unsigned char humi, unsigned char humi_fraction, unsigned char temp, unsigned char temp_fraction)
{
	txbuf[0] = 0x05;
	txbuf[1] = humi;
	txbuf[2] = humi_fraction;
	txbuf[3] = temp; //((temp - 25) << 1) + (((temp - 25) & 0x80) >> 7);
	txbuf[4] = temp_fraction; //(temp << 1) + ((temp & 0x80) >> 7);
	i2c.xmit(address, 5, txbuf, 0, rxbuf);
}

void CCS811::setEnv(float humi, float temp)
{
	setEnv(humi * 2, 0, (temp - 25) * 2, 0);
}

unsigned char CCS811::getStatus()
{
	txbuf[0] = 0x00;
	i2c.xmit(address, 1, txbuf, 1, rxbuf);
	return rxbuf[0];
}

unsigned char CCS811::getError()
{
	txbuf[0] = 0xe0;
	i2c.xmit(address, 1, txbuf, 1, rxbuf);
	return rxbuf[0];
}

// switch to Bootloader mode
void CCS811::reset()
{
	txbuf[0] = 0xff;
	txbuf[1] = 0x11;
	txbuf[2] = 0xe5;
	txbuf[3] = 0x72;
	txbuf[4] = 0x8a;
}

CCS811 ccs811;
