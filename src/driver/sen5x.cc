/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/sen5x.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void SEN5x::cleanFan()
{
	txbuf[0] = 0x56;
	txbuf[1] = 0x07;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SEN5x::start()
{
	txbuf[0] = 0x00;
	txbuf[1] = 0x21;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

void SEN5x::stop()
{
	txbuf[0] = 0x01;
	txbuf[1] = 0x04;
	i2c.xmit(address, 2, txbuf, 0, rxbuf);
}

bool SEN5x::read()
{
	txbuf[0] = 0x03;
	txbuf[1] = 0xc4;

	if (i2c.xmit(address, 2, txbuf, 0, rxbuf)) {
		return false;
	}
	arch.delay_ms(20);
	if (i2c.xmit(address, 0, txbuf, 24, rxbuf)) {
		return false;
	}

	if (!crcValid(rxbuf, 24)) {
		return false;
	}

	pm1 = (rxbuf[0] << 8) + rxbuf[1];
	pm2_5 = (rxbuf[3] << 8) + rxbuf[4];
	pm4 = (rxbuf[6] << 8) + rxbuf[7];
	pm10 = (rxbuf[9] << 8) + rxbuf[10];
	humidity = (rxbuf[12] << 8) + rxbuf[13];
	temperature = (rxbuf[15] << 8) + rxbuf[16];
	voc = (rxbuf[18] << 8) + rxbuf[19];
	nox = (rxbuf[21] << 8) + rxbuf[22];
	return true;
}

bool SEN5x::readStatus()
{
	txbuf[0] = 0xd2;
	txbuf[1] = 0x06;
	if (i2c.xmit(address, 2, txbuf, 0, rxbuf)) {
		return false;
	}
	arch.delay_ms(20);
	if (i2c.xmit(address, 0, txbuf, 6, rxbuf)) {
		return false;
	}

	if (!crcValid(rxbuf, 6)) {
		return false;
	}

	fan_speed_warning = rxbuf[1] & 0x20;
	fan_cleaning_active = rxbuf[1] & 0x08;
	gas_sensor_error = rxbuf[4] & 0x80;
	rht_sensor_error = rxbuf[4] & 0x40;
	laser_failure = rxbuf[4] & 0x20;
	fan_failure = rxbuf[4] & 0x10;

	return true;
}

unsigned char SEN5x::crcWord(unsigned char byte1, unsigned char byte2)
{
	unsigned char crc = 0xff ^ byte1;
	for (unsigned char bit = 8; bit > 0; bit--) {
		crc = (crc << 1) ^ (crc & 0x80 ? 0x31 : 0);
	}
	crc ^= byte2;
	for (unsigned char bit = 8; bit > 0; bit--) {
		crc = (crc << 1) ^ (crc & 0x80 ? 0x31 : 0);
	}
	return crc;
}

bool SEN5x::crcValid(unsigned char* data, unsigned char length)
{
	for (unsigned char i = 0; i < length; i += 3) {
		if (crcWord(data[i], data[i+1]) != data[i+2]) {
			return false;
		}
	}
	return true;
}

SEN5x sen5x;
