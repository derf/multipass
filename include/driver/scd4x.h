/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef SCD4x_H
#define SCD4x_H

class SCD4x {
	private:
		SCD4x(const SCD4x &copy);
		unsigned char const address = 0x62;
		unsigned char txbuf[2];
		unsigned char rxbuf[9];

	public:
		SCD4x() {}

		unsigned short co2;
		unsigned short rawTemperature;
		unsigned short rawHumidity;

		void start();
		void stop();

		void startLowPower();

		bool read();
};

extern SCD4x scd4x;

#endif
