/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif
#include "driver/mpu9250.h"
#ifdef CONFIG_driver_hdc1080
#include "driver/hdc1080.h"
#endif

#define INITI0(val) int val = 0, min_ ## val = 30000, max_ ## val = -30000;
#define INITF0(val) float val = 0; double min_ ## val = 9999, max_ ## val = -9999;
#define UPDATE_MIN(min_val, val) if ((val) < (min_val)) { (min_val) = (val); }
#define UPDATE_MAX(max_val, val) if ((val) > (max_val)) { (max_val) = (val); }

int main(void)
{
	INITF0(ax);
	INITF0(ay);
	INITF0(az);
	INITI0(mx);
	INITI0(my);
	INITI0(mz);
	unsigned short i = 0;

	arch.setup();
	gpio.setup();
	kout.setup();

	if (i2c.setup() != 0) {
		kout << "I2C setup failed" << endl;
		return 1;
	}

	kout << "I2C setup OK" << endl;

#ifdef CONFIG_driver_hdc1080
	hdc1080.init();
	if (hdc1080.getManufacturerID() != 0x5449) {
		kout << "[!] invalid HDC1080 manufacturer ID: " << hex << hdc1080.getManufacturerID() << endl;
	}
#endif

	mpu9250.init();
	mpu9250.nineAxis();
	mpu9250.setGyroEnable(false, false, false);

	while (1) {
		mpu9250.getAccel(&ax, &ay, &az);

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
			kout << "Accel X: " << max_ax - min_ax << endl;
			kout << "Accel Y: " << max_ay - min_ay << endl;
			kout << "Accel Z: " << max_az - min_az << endl;
			kout << "Magnet X: " << max_mx - min_mx << endl;
			kout << "Magnet Y: " << max_my - min_my << endl;
			kout << "Magnet Z: " << max_mz - min_mz << endl;
			kout << "MPU Temperature: " << mpu9250.getTemperature() << endl;
			mpu9250.sleep();

#ifdef CONFIG_driver_hdc1080
			hdc1080.measure();
			arch.delay_ms(10);
			kout << "HDC Temperature: " << hdc1080.getTemp() << endl;
			kout << "HDC Humidity: " << hdc1080.getRH() << endl;
#endif
			return 0;
		}
		arch.delay_ms(1);
	}
}
