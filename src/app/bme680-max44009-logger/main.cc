/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/bme680.h"
#include "driver/bme680_util.h"
#include "driver/max44009.h"

struct bme680_field_data data;

void loop(void)
{
	static unsigned char i = 0;

	if ((i == 1) && (ADCSRA & _BV(ADIF))) {
		uint8_t adcr_l = ADCL;
		uint8_t adcr_h = ADCH;
		uint16_t adcr = adcr_l + (adcr_h << 8);
		uint16_t vcc = 1100L * 1023 / adcr;

		TIFR1 |= _BV(TOV1);
		ADCSRA |= _BV(ADIF);

		kout << "VCC: " << vcc << endl;
	}

	if (i == 0) {
		bme680.setSensorMode();
	}
	else if (i == 1) {
		if (bme680.getSensorData(&data) == 0) {
			kout << "BME680 temperature: " << (float)data.temperature / 100 << " degC" << endl;
			kout << "BME680 humidity: " << (float)data.humidity / 1000  << " %" << endl;
			kout << "BME680 pressure: " << (float)data.pressure / 100 << " hPa" << endl;
			kout << "BME680 gas resistance: " << data.gas_resistance << endl;
		}
		kout << "MAX44009: ";
		kout.printf_float(max44009.getLux());
		kout << " lx" << endl;
	}

	i = (i + 1) % 20;
}

int main(void)
{
	unsigned short i = 0;

	arch.setup();
	gpio.setup();
	kout.setup();

	// One ADC conversion per four seconds
	TCCR0A = 0;
	TCCR0B = _BV(CS12) | _BV(CS10);

	// Measure internal 1.1V bandgap using VCC as reference on each Timer 0 overflow
	ADMUX = _BV(REFS0) | 0x0e;
	ADCSRB = _BV(ADTS2);
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1);

	if (i2c.setup() != 0) {
		kout << "I2C setup failed" << endl;
		return 1;
	}

	kout << "I2C setup OK" << endl;

	bme680.intf = BME680_I2C_INTF;
	bme680.read = bme680_i2c_read;
	bme680.write = bme680_i2c_write;
	bme680.delay_ms = bme680_delay_ms;

	bme680.amb_temp = 25;

	int8_t rslt = BME680_OK;
	rslt = bme680.init();
	kout << "BME680 init " << rslt << endl;

	bme680.power_mode = BME680_FORCED_MODE;
	bme680.tph_sett.os_hum = BME680_OS_2X;
	bme680.tph_sett.os_pres = BME680_OS_16X;
	bme680.tph_sett.os_temp = BME680_OS_2X;

	bme680.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	bme680.gas_sett.heatr_dur = 100;
	bme680.gas_sett.heatr_temp = 300;
	bme680.setSensorSettings(BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL);

	arch.delay_ms(200);

	arch.idle_loop();

	return 0;
}
