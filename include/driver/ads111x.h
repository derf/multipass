/*
 * Copyright 2023 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for ADS111x 16-Bit ADCs
 */
#ifndef ADS111x_H
#define ADS111x_H

#include <stdint.h>

class ADS111x {
	private:
		ADS111x(const ADS111x &copy);
		unsigned char const address;
		unsigned char fsr_scale;

		unsigned char txbuf[3];
		unsigned char rxbuf[2];

		unsigned char const P_CONVERSION = 0;
		unsigned char const P_CONFIG = 1;
		unsigned char const P_LO_THRESH = 2;
		unsigned char const P_HI_THRESH = 3;

	public:
		ADS111x(unsigned char const addr) : address(addr), fsr_scale(8) {}

		unsigned short const START_CONVERSION = 1 << 15;
		enum conf_mux : unsigned short {
			DIFFERENTIAL_01 = 0 << 12,
			DIFFERENTIAL_03 = 1 << 12,
			DIFFERENTIAL_13 = 2 << 12,
			DIFFERENTIAL_23 = 3 << 12,
			SINGLE_0 = 4 << 12,
			SINGLE_1 = 5 << 12,
			SINGLE_2 = 6 << 12,
			SINGLE_3 = 7 << 12
		};
		unsigned short const CONF_PGA_MASK = 0b0000111000000000;
		unsigned short const CONF_PGA_OFFSET = 9;
		enum conf_pga : unsigned short {
			FSR_6V = 0 << 9,
			FSR_4V = 1 << 9,
			FSR_2V = 2 << 9,
			FSR_1V = 3 << 9,
			FSR_0V5 = 4 << 9,
			FSR_0V25 = 5 << 9
		};
		enum conf_mode : unsigned short {
			CONTINUOUS = 0 << 8,
			SINGLE_SHOT = 1 << 8,
		};
		enum conf_data_rate : unsigned short {
			SPS_8 = 0 << 5,
			SPS_16 = 1 << 5,
			SPS_32 = 2 << 5,
			SPS_64 = 3 << 5,
			SPS_128 = 4 << 5,
			SPS_250 = 5 << 5,
			SPS_475 = 6 << 5,
			SPS_860 = 7 << 5
		};
		enum conf_comp_mode : unsigned short {
			TRADITIONAL = 0 << 4,
			WINDOW = 1 << 4
		};
		enum conf_comp_pol : unsigned short {
			ACTIVE_LOW = 0 << 3,
			ACTIVE_HIGH = 1 << 3,
		};
		enum conf_comp_lat : unsigned short {
			NONLATCHING = 0 << 2,
			LATCHING = 1 << 2
		};
		enum conf_comp_queue : unsigned short {
			ONE_CONVERSON = 0,
			TWO_CONVERSIONS,
			FOUR_CONVERSIONS,
			DISABLED
		};

		void configure(unsigned short config);
		int16_t readRaw();
		float readVoltage();
};

extern ADS111x ads111x;

#endif
