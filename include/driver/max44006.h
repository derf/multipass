/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MAX44006_H
#define MAX44006_H

#include <stdint.h>

/**
 * Driver for MAX44006 RGB, IR, and Temperature Sensor.
 * Does not support interrupts.
 */
class MAX44006 {
	public:

		enum registers {
			interruptStatusReg = 0x00,
			mainConfigReg = 0x01,
			ambientConfigReg = 0x02,
		};

		enum interruptStatus {
			AMBINTS = 0b00000001,
			PWRON   = 0b00000010,
			SHDN    = 0b00001000,
			RESET   = 0b00010000,
		};

		enum mainConfig {
			AMBINTE   = 0b00000001,
			AMBSEL_00 = 0b00000000,
			AMBSEL_01 = 0b00000100,
			AMBSEL_10 = 0b00001000,
			AMBSEL_11 = 0b00001100,
			MODE_00   = 0b00000000,
			MODE_01   = 0b00010000,
			MODE_10   = 0b00100000,
		};

		enum ambientConfig {
			AMBPGA_00  = 0b00000000,
			AMBPGA_01  = 0b00000001,
			AMBPGA_10  = 0b00000010,
			AMBPGA_11  = 0b00000011,
			AMBTIM_000 = 0b00000000,
			AMBTIM_001 = 0b00000100,
			AMBTIM_010 = 0b00001000,
			AMBTIM_011 = 0b00001100,
			AMBTIM_100 = 0b00010000,
			TEMPEN     = 0b00100000,
			COMPPEN    = 0b01000000,
			TRIm       = 0b10000000,
		};

	private:
		MAX44006(const MAX44006 &copy);
		unsigned char const address;
		unsigned char txbuf[2];
		unsigned char rxbuf[10];

		unsigned char ambtim;
		unsigned char ambpga;

		uint16_t clear, red, green, blue, ir;

	public:

		MAX44006(unsigned char const addr = 0x45) : address(addr) {}

		uint8_t init();

		uint16_t getTemperature();
		bool getLight(float *red, float *green, float *blue, float *clear, float *ir);
};

extern MAX44006 max44006;

#endif
