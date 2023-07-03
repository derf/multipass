/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef SEN5x_H
#define SEN5x_H

class SEN5x {
	private:
		SEN5x(const SEN5x &copy);
		unsigned char const address = 0x69;
		unsigned char txbuf[2];
		unsigned char rxbuf[24];

		unsigned char crcWord(unsigned char byte1, unsigned char byte2);
		bool crcValid(unsigned char* data, unsigned char length);

	public:
		SEN5x() {}

		unsigned short const PM_INVALID = 0xffff;
		signed short const TEMPERATURE_INVALID = 0x7fff;
		signed short const HUMIDITY_INVALID = 0x7fff;
		signed short const VOC_INVALID = 0x7fff;
		signed short const NOX_INVALID = 0x7fff;

		struct {
			unsigned int fan_speed_warning : 1;
			unsigned int fan_cleaning_active : 1;
			unsigned int gas_sensor_error : 1;
			unsigned int rht_sensor_error : 1;
			unsigned int laser_failure : 1;
			unsigned int fan_failure : 1;
		};

		/*
		 * PM1.0 value, scaled by 10.
		 * PM1.0 [µg/m³] = pm10 / 10
		 */
		unsigned short pm1;

		/*
		 * PM2.5 value, scaled by 10.
		 * PM2.5 [µg/m3] = pm2_5 / 10
		 */
		unsigned short pm2_5;

		/*
		 * PM4.0 value, scaled by 10.
		 * PM4.0 [µg/m3] = pm4 / 10
		 */
		unsigned short pm4;

		/*
		 * PM10 value, scaled by 10.
		 * PM10 [µg/m3] = pm10 / 10
		 */
		unsigned short pm10;

		/*
		 * Temperature, scaled by 200.
		 * Temperature [°c] = temperature / 200
		 */
		signed short temperature;

		/*
		 * Relative Humidity, scaled by 100.
		 * Relative Humidity [%] = humidity / 100
		 */
		signed short humidity;

		/*
		 * VOC Index, scaled by 10.
		 * VOC index = voc / 10
		 */
		signed short voc;

		/*
		 * NOx Index, scaled by 10.
		 * NOx index = nox / 10
		 */
		signed short nox;

		void start();
		void stop();

		void cleanFan();

		bool read();
		bool readStatus();
};

extern SEN5x sen5x;

#endif
