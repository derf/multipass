/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>

#include "arch.h"
#include "driver/adc.h"

uint16_t AVRADC::getPin_mV(uint8_t pin, uint16_t avcc)
{
	if (avcc) {
		// measure with AVCC reference
		ADMUX = _BV(REFS0) | pin;
	} else {
		// measure with internal 1.1V bandgap refernce
		ADMUX = _BV(REFS1) | _BV(REFS0) | pin;
	}

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2);

	// Wait for bandgap to stabilise (70us according to datasheet table 28-3)
	arch.delay_ms(1);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// wait until conversion is complete
	while (ADCSRA & _BV(ADSC)) ;

	uint8_t adcr_l = ADCL;
	uint8_t adcr_h = ADCH;
	uint16_t adcr = adcr_l + (adcr_h << 8);

	// Disable ADC
	ADCSRA &= ~_BV(ADEN);

	if (avcc) {
		return (uint32_t)avcc * adcr / 1023L;
	} else {
		return 1100L * adcr / 1023L;
	}
}

int16_t AVRADC::getTemp_mdegC(int16_t offset)
{
	// Measure temperature probe with 1.1V bandgap reference
	ADMUX = _BV(REFS1) |  _BV(REFS0) | 0x08;

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2);

	// Wait for bandgap to stabilise (70us according to datasheet table 28-3)
	arch.delay_ms(1);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait for conversion to complete
	while (ADCSRA & _BV(ADSC)) ;

	// typical values: 242 mV @ -45 degC
	// typical values: 314 mV @ +25 degC
	// typical values: 380 mV @ +85 degC
	// slope: 0.9090.. degC / mV at  25 .. 85 degC
	// -> approx. 286.5 mV @ 0 degC / approx -260.45 degC @ 0 mV
	// -> T[degC] = ADC[mV] * 0.91 - 261
	// -> T[mdegC] = ADC[mV] * 91 - 26100
	// slope: 0.9722.. mV / degC at -45 .. 25 degC
	// slope: 0.942    mV / degC at -45 .. 85 degC
	uint8_t adcr_l = ADCL;
	uint8_t adcr_h = ADCH;
	uint16_t adcr = adcr_l + (adcr_h << 8);
	uint16_t vadc = 1100L * adcr / 1023L;

	// adjust for chip-specific variations
	vadc += offset;

	int16_t temp_mdegc = vadc * 91 - 26100L;

	// Disable ADC
	ADCSRA &= ~_BV(ADEN);

	return temp_mdegc;
}

uint16_t AVRADC::getVCC_mV()
{
	// Measure internal 1.1V bandgap using VCC as reference
	ADMUX = _BV(REFS0) | 0x0e;

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2);

	// Wait for bandgap to stabilise (70us according to datasheet table 28-3)
	arch.delay_ms(1);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait for conversion to complete
	while (ADCSRA & _BV(ADSC)) ;

	uint8_t adcr_l = ADCL;
	uint8_t adcr_h = ADCH;
	uint16_t adcr = adcr_l + (adcr_h << 8);
	uint16_t vcc = 1100L * 1023 / adcr;

	// Disable ADC
	ADCSRA &= ~_BV(ADEN);

	return vcc;
}

AVRADC adc;
