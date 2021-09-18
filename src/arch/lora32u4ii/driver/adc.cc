#include <avr/io.h>

#include "arch.h"
#include "driver/adc.h"
#include "driver/gpio.h"

int16_t AVRADC::getTemp_mdegC(int16_t offset)
{
	// Measure temperature probe with internal 2.56V bandgap reference
	ADMUX = _BV(REFS1) |  _BV(REFS0) | 0x07;
	ADCSRB = _BV(MUX5);

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);

	// Wait for bandgap and temperature references to stabilise
	arch.delay_ms(1);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait for conversion to complete
	while (ADCSRA & _BV(ADSC)) ;

	// TODO this is for atmega328p, not atmega32u4
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
	ADMUX = _BV(REFS0) | 0x1e;
	ADCSRB = 0;

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);

	// Wait for bandgap to stabilise
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

uint16_t AVRADC::getVBat_mV(bool controlCharger)
{
	// Measure VBat/2 (via voltage divider on PB5 / ADC12) using VCC as reference.
	// Caution: PB5 is also connected to the white user LED.
	ADMUX = _BV(REFS0) | 0x04;
	ADCSRB = _BV(MUX5);

	// Enable ADC with /64 prescaler
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);

	if (controlCharger) {
		// Turn on Vbat/2 voltage divider (and charger!)
		gpio.output(GPIO::pb0, 1);
	}

	// Wait for things to stabilise
	arch.delay_ms(1);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait for conversion to complete
	while (ADCSRA & _BV(ADSC)) ;

	uint8_t adcr_l = ADCL;
	uint8_t adcr_h = ADCH;
	uint16_t adcr = adcr_l + (adcr_h << 8);
	uint16_t vbat = 4200L * adcr / 1023;

	// Disable ADC
	ADCSRA &= ~_BV(ADEN);

	if (controlCharger) {
		// Turn off voltage divider (and charger!)
		gpio.output(GPIO::pb0, 0);
	}

	return vbat;
}


AVRADC adc;
