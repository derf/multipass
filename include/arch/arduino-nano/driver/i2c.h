/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef I2C_H
#define I2C_H

class I2C {
	private:
		I2C(const I2C &copy);

	public:
		I2C () {}
		signed char setup();
		void scan(unsigned int *results);
		signed char xmit(unsigned char address,
				unsigned char tx_len, unsigned char *tx_buf,
				unsigned char rx_len, unsigned char *rx_buf);
};

extern I2C i2c;

#endif
