/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef AM2320_H
#define AM2320_H

/**
 * Driver for AM2320 I2C Temperature and Humidity Sensor.
 * The sensor is in a low-power sleep mode by default. The read() function
 * causes it to wake up, perform a single temperature and humidity reading, and
 * go back to sleeep. Afterwards, the results can be read using getStatus(),
 * getTemp(), and getHumidity().
 */
class AM2320 {
	private:
		AM2320(const AM2320 &copy);
		unsigned char const address;
		unsigned char txbuf[3];
		unsigned char rxbuf[8];

	public:
		/**
		 * The constructor does nothing.
		 * @param addr AM2320 I2C address
		 */
		AM2320(unsigned char const addr) : address(addr) {}

		/**
		 * Perform a single temperature and humidity measurement. Afterwards,
		 * the readings can be accessed using getTemp() and getHumidity().
		 */
		void read();

		/**
		 * Check whether the latest read() operation returned valid data.
		 * @return 0 if valid, non-zero otherwise
		 */
		unsigned char getStatus();

		/**
		 * Return temperature measured by latest read() call.
		 * @return temperature [degree celsius]
		 */
		float getTemp();

		/**
		 * Return humidity measured by latest read() call.
		 * @return relative humidity [percent]
		 */
		float getHumidity();
};

extern AM2320 am2320;

#endif
