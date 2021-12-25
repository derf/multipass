/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for LM75B Digital Temperature Sensor and Thermal Watchdog.
 */
#ifndef LM75_H
#define LM75_H

class LM75 {
	private:
		LM75(const LM75 &copy);
		unsigned char const address;
		unsigned char txbuf[3];
		unsigned char rxbuf[2];

	public:
		LM75(unsigned char const addr) : address(addr) {}

		float getTemp();
		unsigned int getOS();
		unsigned int getHyst();
		void setOS(signed char os);
		void setHyst(signed char hyst);
		void init();
		void shutdown();
};

extern LM75 lm75;

#endif
