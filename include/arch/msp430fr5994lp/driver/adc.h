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

		volatile bool bufferFull;

	public:
		ADC() : bufferFull(false) {}

		float getTemp();
		float getVCC();
		uint16_t getReading();
		void startSampling(uint16_t *buf, uint16_t bufsize);
		void stopSampling();

		inline void setBufferFull()
		{
			bufferFull = true;
		}

		inline bool isBufferFull()
		{
			if (bufferFull) {
				bufferFull = false;
				return true;
			}
			return false;
		}
};

extern ADC adc;

#endif
