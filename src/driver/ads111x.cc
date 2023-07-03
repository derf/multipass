/*
 * Copyright 2023 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for ADS111x 16-Bit ADCs
 */
#include "driver/ads111x.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

void ADS111x::configure(unsigned short config)
{
	txbuf[0] = P_CONFIG;
	txbuf[1] = config >> 8;
	txbuf[2] = config & 0x00ff;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);

	uint8_t pga_config = (config & CONF_PGA_MASK) >> CONF_PGA_OFFSET;
	switch (pga_config) {
		case 0:
			fsr_scale = 24;
			break;
		case 1:
			fsr_scale = 16;
			break;
		case 2:
			fsr_scale = 8;
			break;
		case 3:
			fsr_scale = 4;
			break;
		case 4:
			fsr_scale = 2;
			break;
		default:
			fsr_scale = 1;
	}
}

int16_t ADS111x::readRaw()
{
	txbuf[0] = P_CONVERSION;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	int16_t intermediate = ((int8_t)rxbuf[0]) * 256 + (uint8_t) rxbuf[1];
	return intermediate;
}

float ADS111x::readVoltage()
{
	return readRaw() * 0.256 / 0x7fff * fsr_scale;
}

ADS111x ads111x(0x48);
