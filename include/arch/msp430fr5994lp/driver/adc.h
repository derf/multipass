/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef ADC_H
#define ADC_H

class ADC {
	private:
		ADC(ADC const &copy);

	public:
		ADC() {}

		float getTemp();
		float getVCC();
};

extern ADC adc;

#endif
