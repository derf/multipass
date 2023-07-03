/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/am2320.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif
#include "arch.h"

void AM2320::read()
{
	txbuf[0] = 0;
	i2c.xmit(address, 1, txbuf, 0, rxbuf);
	arch.delay_ms(1);
	txbuf[0] = 3;
	txbuf[1] = 0;
	txbuf[2] = 4;
	rxbuf[3] = rxbuf[4] = rxbuf[5] = rxbuf[6] = 0;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
	arch.delay_ms(3);
	i2c.xmit(address, 0, txbuf, 8, rxbuf);
}

unsigned char AM2320::getStatus()
{
	if (rxbuf[0] != 3) {
		return 1;
	}
	if (rxbuf[1] != 4) {
		return 2;
	}
	unsigned short checksum = 0xffff;
	for (unsigned char i = 0; i < 6; i++) {
		checksum ^= rxbuf[i] & 0x00ff;
		for (unsigned char j = 0; j < 8; j++) {
			if (checksum & 0x0001) {
				checksum = (checksum >> 1) ^ 0xa001;
			} else {
				checksum >>= 1;
			}
		}
	}
	if ((rxbuf[6] != (checksum & 0x00ff)) || (rxbuf[7] != (checksum >> 8))) {
		return 3;
	}
	return 0;
}

float AM2320::getTemp()
{
	if (rxbuf[5] & 0x80) {
		return (-256 * (rxbuf[4] & 0x7f) + rxbuf[5]) / 10.;
	}
	return (256 * rxbuf[4] + rxbuf[5]) / 10.;
}

float AM2320::getHumidity()
{
	return (rxbuf[2] * 256 + rxbuf[3]) / 10.;
}

AM2320 am2320(0x5c);
