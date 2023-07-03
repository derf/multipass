/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef CCS811_H
#define CCS811_H

class CCS811 {
	private:
		CCS811(const CCS811 &copy);
		unsigned char const address = 0x5a;
		unsigned char txbuf[5];
		unsigned char rxbuf[8];

	public:
		CCS811() {}

		unsigned short eco2;
		unsigned short tvoc;
		unsigned char status;
		unsigned char error_id;
		unsigned char raw_current;
		unsigned short raw_voltage;

		void init();
		unsigned char getStatus();
		unsigned char getError();
		unsigned char getManufacturerID();
		void startFirmware();
		void setMode(unsigned char mode);
		void read();
		void setEnv(unsigned char humi, unsigned char humi_fraction, unsigned char temp, unsigned char temp_fraction);
		void setEnv(float humi, float temp);
		void reset();
};

extern CCS811 ccs811;

#endif
