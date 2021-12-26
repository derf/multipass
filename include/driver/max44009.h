/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MAX44009_H
#define MAX44009_H

/**
 * Driver for MAX44009 Ambient Light Sensor.
 * Does not support interrupts.
 */
class MAX44009 {
	private:
		MAX44009(const MAX44009 &copy);
		unsigned char const address;
		unsigned char txbuf[2];
		unsigned char rxbuf[2];

	public:
		/**
		 * Create a new MAX44009 object for the specified I2C address.
		 * This is a no-op; the sensor is not initialized. In its default
		 * configuration, it takes a light reading every 800ms and uses
		 * auto-ranging to select a suitable measurement range.
		 *
		 * @param addr I2C address of light sensor, default 0x4a
		 */
		MAX44009(unsigned char const addr = 0x4a) : address(addr) {}

		/**
		 * Retrieve latest ambient light reading from the sensor.
		 * @return ambient light [lx]; -1 if a communication error or overrange
		 *         condition occured.
		 */
		float getLux();
};

extern MAX44009 max44009;

#endif
