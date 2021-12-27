/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for VEML6075 Ambient Light Sensor.
 */
#include "driver/veml6075.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

bool VEML6075::init()
{
	txbuf[0] = 0x00;
	txbuf[1] = 0x00; // 50ms integration time, normal dynamic
	if (i2c.xmit(address, 2, txbuf, 0, rxbuf) != 0) {
		return false;
	}
	return true;
}

bool VEML6075::readUVCounts(float *uva, float *uvb)
{
	uint16_t uva_counts, uvb_counts, comp_visible, comp_ir;
	txbuf[0] = 0x07;
	if (i2c.xmit(address, 2, txbuf, 2, rxbuf) != 0) {
		return false;
	}
	uva_counts = ((uint16_t)rxbuf[1] << 8) + rxbuf[0];

	txbuf[0] = 0x09;
	if (i2c.xmit(address, 2, txbuf, 2, rxbuf) != 0) {
		return false;
	}
	uvb_counts = ((uint16_t)rxbuf[1] << 8) + rxbuf[0];

	txbuf[0] = 0x0a;
	if (i2c.xmit(address, 2, txbuf, 2, rxbuf) != 0) {
		return false;
	}
	comp_visible = ((uint16_t)rxbuf[1] << 8) + rxbuf[0];

	txbuf[0] = 0x0b;
	if (i2c.xmit(address, 2, txbuf, 2, rxbuf) != 0) {
		return false;
	}
	comp_ir = ((uint16_t)rxbuf[1] << 8) + rxbuf[0];

	*uva = uva_counts - uva_a_coef * comp_visible - uva_b_coef * comp_ir;
	*uvb = uvb_counts - uvb_c_coef * comp_visible - uvb_d_coef * comp_ir;

	return true;
}

bool VEML6075::readUV(float *uva, float *uvb)
{
	float uva_counts, uvb_counts;
	if (!readUVCounts(&uva_counts, &uvb_counts)) {
		return false;
	}

	*uva = uva_counts * uva_counts_per_uwcm2;
	*uvb = uvb_counts * uvb_counts_per_uwcm2;
	return true;
}

bool VEML6075::readUVI(float *uva, float *uvb)
{
	float uva_counts, uvb_counts;
	if (!readUVCounts(&uva_counts, &uvb_counts)) {
		return false;
	}

	*uva = uva_counts * uva_uvi_response;
	*uvb = uvb_counts * uvb_uvi_response;
	return true;
}

VEML6075 veml6075(0x10);
