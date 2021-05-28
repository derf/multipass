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
#include "driver/adc.h"
#include "driver/mpu9250.h"

#define INIT0(val) int val = 0, min_ ## val = 0, max_ ## val = 0;
#define UPDATE_MIN(min_val, val) if ((val) < (min_val)) { (min_val) = (val); }
#define UPDATE_MAX(max_val, val) if ((val) > (max_val)) { (max_val) = (val); }

unsigned char button = 0;

void check_button(GPIO::Pin pin, unsigned char index)
{
	if (gpio.read(pin) == 0) {
		if ((button & (1 << index)) == 0) {
			kout << "button" << index << endl;
		}
		button |= 1 << index;
	} else {
		button &= ~(1 << index);
	}
	
}

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

	gpio.input(GPIO::pb0, 1);
	gpio.input(GPIO::pb1, 1);
	gpio.input(GPIO::pb2, 1);
	gpio.input(GPIO::pb3, 1);
	gpio.input(GPIO::pb4, 1);

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

		check_button(GPIO::pb4, 0);
		check_button(GPIO::pb3, 1);
		check_button(GPIO::pb2, 2);
		check_button(GPIO::pb1, 3);
		check_button(GPIO::pb0, 4);

		if (i++ == 2000) {
			kout << "Min Accel: " << min_ax << " / " << min_ay << " / " << min_az << endl;
			kout << "Max Accel: " << max_ax << " / " << max_ay << " / " << max_az << endl;
			kout << "Min Magnet: " << min_mx << " / " << min_my << " / " << min_mz << endl;
			kout << "Max Magnet: " << max_mx << " / " << max_my << " / " << max_mz << endl;
			kout << "MPU Temp: " << mpu9250.getTemperature() << endl;
			kout << "CPU Temp: " << adc.getTemp_mdegC() << endl;
			kout << "CPU VCC: " << adc.getVCC_mV() << endl;
			min_ax = min_ay = min_az = 30000;
			max_ax = max_ay = max_az = -30000;
			min_mx = min_my = min_mz = 30000;
			max_mx = max_my = max_mz = -30000;
			i = 0;
		}
		arch.delay_ms(1);
	}

	return 0;
}
