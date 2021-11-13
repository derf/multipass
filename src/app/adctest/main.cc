/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

void loop(void)
{
	uint16_t vcc = adc.getVCC_mV();
	uint16_t temp = adc.getTemp_mdegC();

	kout << "Voltage     " << vcc << " mv" << endl;
	kout << "Temperature " << temp << " m°C" <<  endl;

	for (uint8_t admux_sel = 0; admux_sel < 8; admux_sel++) {
		// measure with avcc reference
		ADMUX = _BV(REFS0) | admux_sel;

		// Enable ADC with /64 prescaler
		ADCSRA = _BV(ADEN) | _BV(ADPS2);

		// Start conversion
		ADCSRA |= _BV(ADSC);

		// wait until conversion is complete
		while (ADCSRA & _BV(ADSC)) ;

		uint8_t adcr_l = ADCL;
		uint8_t adcr_h = ADCH;
		uint16_t adcr = adcr_l + (adcr_h << 8);
		uint16_t vadc = (uint32_t)vcc * adcr / 1023L;

		kout << "ADC" << admux_sel << "        " << vadc << " mV vs AVCC" << endl;
	}

	// enable bandgap; wait for it to stabilise
	ADMUX = _BV(REFS1) |  _BV(REFS0);
	ADCSRA = _BV(ADEN) | _BV(ADPS2);
	arch.delay_ms(1);

	for (uint8_t admux_sel = 0; admux_sel < 8; admux_sel++) {
		// measure with bandgap reference
		ADMUX = _BV(REFS1) | _BV(REFS0) | admux_sel;

		// Enable ADC with /64 prescaler
		ADCSRA = _BV(ADEN) | _BV(ADPS2);

		// Start conversion
		ADCSRA |= _BV(ADSC);

		// wait until conversion is complete
		while (ADCSRA & _BV(ADSC)) ;

		uint8_t adcr_l = ADCL;
		uint8_t adcr_h = ADCH;
		uint16_t adcr = adcr_l + (adcr_h << 8);
		uint16_t vadc = 1100L * adcr / 1023L;

		kout << "ADC" << admux_sel << "        " << vadc << " mV vs 1.1V bandgap" << endl;
	}

	// disable ADC
	ADCSRA &= ~_BV(ADEN);
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	arch.idle_loop();

	return 0;
}
