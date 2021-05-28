/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TSL2591_H
#define TSL2591_H

class TSL2591 {
	private:
		TSL2591(const TSL2591 &copy);
		unsigned char const address = 0x29;
		unsigned char txbuf[2];
		unsigned char rxbuf[4];

	public:
		TSL2591() {}

		unsigned short ch0;
		unsigned short ch1;

		void init();

		unsigned char getStatus();
		void read();
};

extern TSL2591 tsl2591;

#endif
