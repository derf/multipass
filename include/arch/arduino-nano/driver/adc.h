/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef ADC_H
#define ADC_H

#include <stdint.h>

class AVRADC {
	private:
		AVRADC(AVRADC const &copy);

	public:
		AVRADC() {}

		int16_t getTemp_mdegC(int16_t offset = 205);
		uint16_t getVCC_mV();
		uint16_t getPin_mV(uint8_t pin, uint16_t avcc = 0);
};

extern AVRADC adc;

#endif
