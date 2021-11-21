/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef I2C_H
#define I2C_H

class I2C {
	private:
		I2C(const I2C &copy);
		const char *i2cbus;

	public:
		I2C (const char *i2cbus) : i2cbus(i2cbus) {}
		signed char setup();
		signed char xmit(unsigned char address,
				unsigned char tx_len, unsigned char *tx_buf,
				unsigned char rx_len, unsigned char *rx_buf);
};

extern I2C i2c;

#endif
