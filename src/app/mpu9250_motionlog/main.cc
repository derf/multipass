/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/mpu9250.h"

#define INIT0(val) int val = 0, min_ ## val = 0, max_ ## val = 0;
#define UPDATE_MIN(min_val, val) if ((val) < (min_val)) { (min_val) = (val); }
#define UPDATE_MAX(max_val, val) if ((val) > (max_val)) { (max_val) = (val); }

int main(void)
{
	INIT0(ax);
	INIT0(ay);
	INIT0(az);
	INIT0(mx);
	INIT0(my);
	INIT0(mz);
	unsigned short i = 0;

	arch.setup();
	gpio.setup();
	kout.setup();

	// One ADC conversion per four seconds
	TCCR1A = 0;
	TCCR1B = _BV(CS12) | _BV(CS10);

	// Measure internal 1.1V bandgap using VCC as reference on each Timer 1 overflow
	ADMUX = _BV(REFS0) | 0x0e;
	ADCSRB = _BV(ADTS2) | _BV(ADTS1);
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1);

	if (i2c.setup() != 0) {
		kout << "I2C setup failed" << endl;
		return 1;
	}

	kout << "I2C setup OK" << endl;

	mpu9250.init();
	mpu9250.nineAxis();
	mpu9250.setGyroEnable(false, false, false);

	while (1) {
		mpu9250.getRawAccel(&ax, &ay, &az);

		UPDATE_MIN(min_ax, ax);
		UPDATE_MIN(min_ay, ay);
		UPDATE_MIN(min_az, az);
		UPDATE_MAX(max_ax, ax);
		UPDATE_MAX(max_ay, ay);
		UPDATE_MAX(max_az, az);

		if (mpu9250.getRawMagnet(&mx, &my, &mz)) {
			UPDATE_MIN(min_mx, mx);
			UPDATE_MIN(min_my, my);
			UPDATE_MIN(min_mz, mz);
			UPDATE_MAX(max_mx, mx);
			UPDATE_MAX(max_my, my);
			UPDATE_MAX(max_mz, mz);
		}

		if (i++ == 2000) {
			kout << "Min Accel: " << min_ax << " / " << min_ay << " / " << min_az << endl;
			kout << "Max Accel: " << max_ax << " / " << max_ay << " / " << max_az << endl;
			kout << "Min Magnet: " << min_mx << " / " << min_my << " / " << min_mz << endl;
			kout << "Max Magnet: " << max_mx << " / " << max_my << " / " << max_mz << endl;
			kout << "Temp: " << mpu9250.getTemperature() << endl;
			min_ax = min_ay = min_az = 30000;
			max_ax = max_ay = max_az = -30000;
			min_mx = min_my = min_mz = 30000;
			max_mx = max_my = max_mz = -30000;
			i = 0;
			if (ADCSRA & _BV(ADIF)) {
				uint8_t adcr_l = ADCL;
				uint8_t adcr_h = ADCH;
				uint16_t adcr = adcr_l + (adcr_h << 8);
				uint16_t vcc = 1100L * 1023 / adcr;

				TIFR1 |= _BV(TOV1);
				ADCSRA |= _BV(ADIF);

				kout << "VCC: " << vcc << endl;
			}
		}
		arch.delay_ms(1);
	}

	return 0;
}
