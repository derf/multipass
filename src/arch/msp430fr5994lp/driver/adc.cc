/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/adc.h"
#include <msp430.h>

#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)
#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)

float ADC::getTemp()
{
	float ret;

	while(REFCTL0 & REFGENBUSY);

	REFCTL0 = REFVSEL_0 | REFON;
	ADC12CTL0 &= ~ADC12ENC;
	ADC12CTL0 = ADC12SHT0_8 | ADC12ON;
	ADC12CTL1 = ADC12SHP;
	ADC12CTL3 = ADC12TCMAP;
	ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_30;
	while(!(REFCTL0 & REFGENRDY));

	ADC12CTL0 |= ADC12ENC;
	ADC12CTL0 |= ADC12SC;
	while (ADC12CTL1 & ADC12BUSY);

	ret = (float)((long)ADC12MEM0 - CALADC12_12V_30C) * (85 - 30) /
		(CALADC12_12V_85C - CALADC12_12V_30C) + 30.0f;

	// Disable ADC
	ADC12CTL0 &= ~ADC12ENC; // disable any conversion to allow ADC configuration
	ADC12CTL0 &= ~ADC12ON; // Turn off ADC

	// Disable internal 2V reference
	while(REFCTL0 & REFGENBUSY);
	REFCTL0 &= ~REFON;

	return ret;
}

float ADC::getVCC()
{
	float ret;

	while(REFCTL0 & REFGENBUSY);

	REFCTL0 = REFVSEL_1 | REFON;
	ADC12CTL0 &= ~ADC12ENC;
	ADC12CTL0 = ADC12SHT0_8 | ADC12ON;
	ADC12CTL1 = ADC12SHP;
	ADC12CTL3 = ADC12BATMAP;
	ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_31;
	while(!(REFCTL0 & REFGENRDY));

	ADC12CTL0 |= ADC12ENC;
	ADC12CTL0 |= ADC12SC;
	while (ADC12CTL1 & ADC12BUSY);

	ret = (float)ADC12MEM0 / 4096 * 2 * 2;
	return ret;

	// Disable ADC
	ADC12CTL0 &= ~ADC12ENC; // disable any conversion to allow ADC configuration
	ADC12CTL0 &= ~ADC12ON; // Turn off ADC

	// Disable internal 2V reference
	while(REFCTL0 & REFGENBUSY);
	REFCTL0 &= ~REFON;

	return ret;
}

ADC adc;
