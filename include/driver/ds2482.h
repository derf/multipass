/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * DS2482-100 Single-Channel 1-Wire Master
 */
#pragma once

class DS2482 {
	private:
		DS2482(const DS2482 &copy);
		unsigned char const address;
		unsigned char txbuf[2];
		unsigned char rxbuf[1];

	public:
		DS2482(unsigned char const addr) : address(addr) {}

		void setup();
		void busReset();
		unsigned char status();
		void readROM(unsigned char *data, unsigned char len);
};

extern DS2482 ds2482;
