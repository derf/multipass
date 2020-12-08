/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
void CPUFreq::set(unsigned int freq_khz)
{
	/*
	 * Note: arch drivers assume SMCLK freq == F_CPU
	 */

	if (freq_khz == 16000) {
		FRCTL0 = FWPW; // unlock FRAM Control
		FRCTL0_L = 0x10; // one wait state before FRAM access (required for 8MHz < F_CPU <= 16 MHz)
		FRCTL0_H = 0xff; // lock FRAM control by writing an invalid password
	}

	CSCTL0_H = CSKEY >> 8;
	if (freq_khz == 16000) {
		CSCTL1 = DCORSEL | DCOFSEL_4;
	} else if (freq_khz == 8000) {
		CSCTL1 = DCOFSEL_6;
	} else if (freq_khz == 4000) {
		CSCTL1 = DCOFSEL_3;
	} else if (freq_khz == 1000) {
		CSCTL1 = DCOFSEL_0;
	}
	CSCTL0_H = 0;
}
