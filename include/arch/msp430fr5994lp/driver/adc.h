/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef ADC_H
#define ADC_H

#include <stddef.h>
#include <stdint.h>

class ADC {
	private:
		ADC(ADC const &copy);

		uint16_t bufsize;
		uint16_t bufpos;
		uint16_t *buf16;
		uint8_t *buf8;
		volatile bool bufferFull;

	public:
		ADC() : bufsize(0), bufpos(0), buf16(NULL), buf8(NULL), bufferFull(false) {}

		float getTemp();
		float getVCC();
		uint16_t getReading();
		void startSampling(uint16_t *buf, uint16_t bufsize);
		void stopSampling();

		inline bool doneSampling()
		{
			if (bufferFull) {
				bufferFull = false;
				return true;
			}
			return false;
		}

		inline void storeReading(uint16_t value)
		{
			buf16[bufpos++] = value;
			bufpos %= bufsize;
			if (bufpos == 0) {
				bufferFull = true;
			}
		}
};

extern ADC adc;

#endif
