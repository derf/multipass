/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include <stdint.h>

void bme680_delay_ms(uint32_t const period)
{
	arch.delay_ms(period);
}

void bme680_sleep_ms(uint32_t const period)
{
#if defined(CONFIG_arch_posix)
	arch.delay_ms(period);
#else
	arch.sleep_ms(period);
#endif
}

int8_t bme680_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t ret = i2c.xmit(dev_id, 1, &reg_addr, 0, 0);
	if (ret) {
		return ret;
	}
	return i2c.xmit(dev_id, 0, 0, len, reg_data);
}

int8_t bme680_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	unsigned char txbuf[len+1];
	txbuf[0] = reg_addr;
	for (uint16_t i = 0; i < len; i++) {
		txbuf[i+1] = reg_data[i];
	}
	return i2c.xmit(dev_id, len + 1, txbuf, 0, 0);
}
