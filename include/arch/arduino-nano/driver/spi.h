/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef SPI_H
#define SPI_H

class SPI {
	private:
		SPI(const SPI &copy);

	public:
		SPI () {}
		signed char setup();
		signed char xmit(
				unsigned char tx_len, unsigned char *tx_buf,
				unsigned char rx_len, unsigned char *rx_buf);
};

extern SPI spi;

#endif
