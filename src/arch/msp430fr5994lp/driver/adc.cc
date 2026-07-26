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
	ADC12CTL0 = ADC12SHT0_8 | ADC12ON; // Sample and Hold: 256 ADC12CLK Cycles
	ADC12CTL1 = ADC12SHP;
	ADC12CTL3 = ADC12TCMAP;
	ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_30;
	while(!(REFCTL0 & REFGENRDY));

	ADC12CTL0 |= ADC12ENC | ADC12SC;
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
	ADC12CTL0 = ADC12SHT0_8 | ADC12ON; // Sample and Hold: 256 ADC12CLK Cycles
	ADC12CTL1 = ADC12SHP;
	ADC12CTL3 = ADC12BATMAP;
	ADC12MCTL0 = ADC12VRSEL_1 | ADC12INCH_31;
	while(!(REFCTL0 & REFGENRDY));

	ADC12CTL0 |= ADC12ENC | ADC12SC;
	while (ADC12CTL1 & ADC12BUSY);

	ret = (float)ADC12MEM0 / 4096 * 2 * 2;

	// Disable ADC
	ADC12CTL0 &= ~ADC12ENC; // disable any conversion to allow ADC configuration
	ADC12CTL0 &= ~ADC12ON; // Turn off ADC

	// Disable internal 2V reference
	while(REFCTL0 & REFGENBUSY);
	REFCTL0 &= ~REFON;

	return ret;
}

uint16_t ADC::getReading()
{
	ADC12CTL0 &= ~ADC12ENC; // disable ADC for configuration
	ADC12CTL0 = ADC12SHT0_8 | ADC12SHT1_8 | ADC12ON; // Sample & Hold: 64 ADC12CLK cycles; enable ADC
	ADC12CTL1 = ADC12SHP; // MODOSC, no prescaler
	ADC12CTL2 = ADC12RES_2; // 12 bit resolution
	ADC12CTL3 = ADC12CSTARTADD_0; // store reading in ADCMEM0

	// disable output driver and input schmitt triggers on P3.0 (A12)
	P3SEL0 |= BIT0;
	P3SEL1 |= BIT0;

	// use default reference (AVSS / AVCC) (only required if changed by some other runtime code beforehand)
	// read from A12 (P3.0)
	ADC12MCTL0 = ADC12INCH_12;

	// get reading
	ADC12CTL0 |= ADC12ENC | ADC12SC;
	while (ADC12CTL1 & ADC12BUSY);

	uint16_t ret = ADC12MEM0;

	// Disable ADC
	ADC12CTL0 &= ~ADC12ENC; // disable any conversion to allow ADC configuration
	ADC12CTL0 &= ~ADC12ON; // Turn off ADC

	return ret;
}

ADC adc;
