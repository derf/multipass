/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef VEML6075_H
#define VEML6075_H

#include <stdint.h>

/**
 * Driver for VEML6075 UV Sensor.
 */
class VEML6075 {
	private:
		VEML6075(const VEML6075 &copy);
		unsigned char const address;
		unsigned char txbuf[2];
		unsigned char rxbuf[2];

		/*
		 * Calibration data from <https://cdn.sparkfun.com/assets/3/9/d/4/1/designingveml6075.pdf>,
		 * used to remove the visible and infrared response from UVA / UVB readings.
		 */
		const float uva_a_coef = 2.22;
		const float uva_b_coef = 1.33;
		const float uvb_c_coef = 2.95;
		const float uvb_d_coef = 1.74;

		/*
		 * Datasheet values for counts → µW/cm² conversion at 50ms integration time.
		 * I'm not sure whether this is before or after adjusting for visible and IR response.
		 */
		const float uva_counts_per_uwcm2 = 0.93;
		const float uvb_counts_per_uwcm2 = 2.1;

		/*
		 * Responsivity for UV Index calculation from <https://cdn.sparkfun.com/assets/3/9/d/4/1/designingveml6075.pdf>.
		 * Used after visible/IR compensation.
		 */
		const float uva_uvi_response = 0.001461;
		const float uvb_uvi_response = 0.002591;

		bool readUVCounts(float *uva, float *uvb);

	public:
		/**
		 * Create a new VEML6075 object for the specified I2C address.
		 * This is a no-op; the sensor is not initialized. In its default
		 * configuration, it takes a light reading every 800ms and uses
		 * auto-ranging to select a suitable measurement range.
		 *
		 * @param addr I2C address of light sensor, default 0x4a
		 */
		VEML6075(unsigned char const addr = 0x10) : address(addr) {}

		/**
		 * Initialize VEML6075 to power on, normal mode, normal dynamic, 50ms
		 * integration time.
		 * @return true if initialization was successful.
		 */
		bool init();

		/**
		 * Read UVA/UVB irradiance.
		 * @param uva UVA irradiance [µW/cm²]
		 * @param uvb UVB irradiance [µW/cm²]
		 * @return true if readout was successful.
		 */
		bool readUV(float *uva, float *uvb);

		/**
		 * Read UVA/UVB Index. 0 is no UV, higher values indicate increased UV exposure.
		 * The overall UV index is mean(UVA Index, UV Index) == (UVA Index + UVB Index)/2.
		 * @param uva UVA Index
		 * @param uvb UVB Index
		 * @return true if readout was successful.
		 */
		bool readUVI(float *uva, float *uvb);
};

extern VEML6075 veml6075;

#endif
