/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/adc.h"
#include <msp430.h>

#define CALADC12_12V_30C  *((unsigned int *)0x1A1A)
#define CALADC12_12V_85C  *((unsigned int *)0x1A1C)

/*
 * Obtain a single reading of the embedded temperature sensor, converted to °C.
 * Configures and enables ADC and voltage reference, reads the temperature, and then disables both.
 * This is a blocking function.
 */
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


/*
 * Obtain a single VCC reading, using the internal 2V voltage reference and the internal VCC/2 rail.
 * Configures and enables ADC and voltage reference, reads the temperature, and then disables both.
 * This is a blocking function.
 */
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


/*
 * Obtain a single 12-bit reading of A12 (P3.0), using AVCC / AVSS as references.
 * Configures and enables the ADC, reads a single value, and then disables the ADC again.
 * This is a blocking function.
 */
uint16_t ADC::getReading()
{
	ADC12CTL0 &= ~ADC12ENC; // disable ADC for configuration
	ADC12CTL0 = ADC12SHT0_8 | ADC12SHT1_8 | ADC12ON; // Sample & Hold: 256 ADC12CLK cycles; enable ADC
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

/*
 * Obtain a serias of 12-bit readings of A12 (P3.0), using AVCC / AVSS as references.
 * Configures and enables the ADC, and then returns.
 */
void ADC::startSampling(uint16_t *buf, uint16_t bufsize)
{
	ADC12CTL0 &= ~ADC12ENC; // disable ADC for configuration
	ADC12CTL0 = ADC12SHT0_8 | ADC12SHT1_8 | ADC12MSC | ADC12ON; // Sample & Hold: 64 ADC12CLK cycles; multiple conversions; enable ADC

	/*
	 * Sample & Hold: 64 ADC12CLK cycles
	 * 8-bit reading: 10 ADC12CLK cycles
	 * → 74 ADC12CLK cycles per reading
	 * → ≈68 kHz sample rate at ≈5 MHz
	 * 216,216 Hz / 2048 == 106 Hz (close enough)
	 * → set prescalers to /2048 (or something close, anyways)
	 * ... lowest we can get with SMCLK ref is /512, that's fine too
	 * Also, we want repeated single-channel conversion.
	 */
	ADC12CTL1 = ADC12PDIV__64 | ADC12SHP | ADC12DIV_7 | ADC12SSEL_3 | ADC12CONSEQ_2;

	// 12 bit readings (14 clock cycles conversion time)
	// (10 bit → 12 cycles / 12 bit → 14 cycles)
	ADC12CTL2 = ADC12RES_2;

	// store each (consecutive) reading in ADCMEM1
	ADC12CTL3 = ADC12CSTARTADD_1;

	// use default reference (AVSS / AVCC) (only required if changed by some other runtime code beforehand)
	// read from A12 (P3.0)
	ADC12MCTL1 = ADC12INCH_12;

	// disable output driver and input schmitt triggers on P3.0 (A12)
	P3SEL0 |= BIT0;
	P3SEL1 |= BIT0;

	// raise an interrupt after each sample
	ADC12IER0 = ADC12IE1;
	ADC12IFGR0 = ADC12IFG1;

	// clear pending interrupts
	ADC12IV = 0;

	buf16 = buf;
	this->bufsize = bufsize;

	ADC12CTL0 |= ADC12ENC | ADC12SC;
}

void ADC::stopSampling()
{
	ADC12CTL0 &= ~ADC12ENC; // disable any conversion to allow ADC configuration
	ADC12CTL0 &= ~ADC12ON; // Turn off ADC
}

ADC adc;

#ifndef __acweaving
__attribute__((interrupt(ADC12_VECTOR))) void handle_adc_irq()
{
	if (ADC12IV == 0x00e) {
		// ADC12MEM1
		adc.storeReading(ADC12MEM1);
	}
}
#endif
