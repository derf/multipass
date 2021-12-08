/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef HDC1080_H
#define HDC1080_H

class HDC1080 {
	private:
		HDC1080(const HDC1080 &copy);
		unsigned char const address = 0x40;
		unsigned char txbuf[3];
		unsigned char rxbuf[2];

	public:
		HDC1080() {}

		float getTemp();
		float getRH();
		unsigned int getManufacturerID();
		void init();
		void measure();
		void heater(bool on);
};

extern HDC1080 hdc1080;

#endif
