/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#include <avr/interrupt.h>

volatile uint16_t anemometer_count = 0;

void loop(void)
{
	static uint8_t wind_count = 0;
	static uint8_t dew_count = 0;
	static uint16_t anemometer_copy;

	wind_count++;
	dew_count++;

	if (wind_count == 10) {

		cli();
		anemometer_copy = anemometer_count;
		anemometer_count = 0;
		sei();

		kout << "Anemometer Count = " << anemometer_copy << endl;
		wind_count = 0;
	}

	if (dew_count == 59) {
		gpio.output(GPIO::pc5, 0);
		gpio.output(GPIO::pc4, 1);
	}
	else if (dew_count == 60) {
		// Measure ADC6 with AVCC reference
		ADMUX = _BV(REFS0) | 6;

		// Enable ADC with /64 prescaler
		ADCSRA = _BV(ADEN) | _BV(ADPS2);

		// Start conversion
		ADCSRA |= _BV(ADSC);

		// wait until conversion is complete
		while (ADCSRA & _BV(ADSC)) ;

		uint8_t adcr_l = ADCL;
		uint8_t adcr_h = ADCH;
		uint16_t dew_raw = adcr_l + (adcr_h << 8);
		dew_raw = 1024 - dew_raw;

		// Disable ADC
		ADCSRA &= ~_BV(ADEN);

		kout << "Dew Raw = " << dew_raw << endl;

		dew_count = 0;
		gpio.input(GPIO::pc5, 0);
		gpio.input(GPIO::pc4, 0);
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.output(GPIO::pd2, 0); // Anemometer Reed Contact
	gpio.input(GPIO::pd3, 1); // Anemometer Reed Contact

	gpio.input(GPIO::pc5, 0); // Dew Sensor GND
	gpio.input(GPIO::pc4, 0); // Dew Sensor VCC

	EICRA = _BV(ISC11);
	EIMSK = _BV(INT1);

	arch.idle_loop();

	return 0;
}

ISR(INT1_vect) {
	anemometer_count++;
}
