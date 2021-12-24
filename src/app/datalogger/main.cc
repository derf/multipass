/*
 * Copyright 2020 Daniel Friesel
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

#ifdef CONFIG_driver_lm75
#include "driver/lm75.h"
#endif
#ifdef CONFIG_driver_s5851a
#include "driver/s5851a.h"
#endif
#ifdef CONFIG_driver_am2320
#include "driver/am2320.h"
#endif
#ifdef CONFIG_driver_bme280
#include "driver/bme280.h"
#include "driver/bme680_util.h"
#endif
#ifdef CONFIG_driver_bme680
#include "driver/bme680.h"
#include "driver/bme680_util.h"
#endif
#ifdef CONFIG_driver_ccs811
#include "driver/ccs811.h"
#endif
#ifdef CONFIG_driver_ds2482
#include "driver/ds2482.h"
#endif
#ifdef CONFIG_driver_max44009
#include "driver/max44009.h"
#endif
#ifdef CONFIG_driver_hdc1080
#include "driver/hdc1080.h"
#endif
#ifdef CONFIG_driver_mpu9250
#include "driver/mpu9250.h"
#endif
#ifdef CONFIG_driver_tsl2591
#include "driver/tsl2591.h"
#endif
#ifdef CONFIG_driver_scd4x
#include "driver/scd4x.h"
#endif

void loop(void)
{
#ifdef CONFIG_driver_lm75
	kout << "temperature_celsius: ";
	kout.printf_float(lm75.getTemp());
	kout << endl;
#endif

#ifdef CONFIG_driver_s5851a
	kout << "temperature_celsius: ";
	kout.printf_float(s5851a.getTemp());
	kout << endl;
#endif

#ifdef CONFIG_driver_am2320
	am2320.read();
	if (am2320.getStatus() == 0) {
		kout.printf_float(am2320.getTemp());
		kout << " degC @ ";
		kout.printf_float(am2320.getHumidity());
		kout << " rel%" << endl;
	} else {
		kout << "AM2320 error " << dec << am2320.getStatus() << endl;
	}
#endif

#ifdef CONFIG_driver_bme280
	struct bme280_data comp_data;
	int8_t rslt = bme280.getSensorData(BME280_ALL, &comp_data);
	kout << "BME280 read " << rslt << endl;
	kout << "BME280 temperature " << (float)comp_data.temperature / 100 << " degC" << endl;
	kout << "BME280 humidity " << (float)comp_data.humidity / 1024 << " %" << endl;
	kout << "BME280 pressure " << (float)comp_data.pressure / 100 << " Pa" << endl;
#endif

#ifdef CONFIG_driver_bme680
	struct bme680_field_data data;
	bme680.setSensorMode();
	arch.delay_ms(250);
	bme680.getSensorData(&data);
	kout << "BME680 temperature " << (float)data.temperature / 100 << " degC" << endl;
	kout << "BME680 humidity " << (float)data.humidity / 1000  << " %" << endl;
	kout << "BME680 pressure " << (float)data.pressure / 100 << " hPa" << endl;
	kout << "BME680 gas resistance " << data.gas_resistance << endl;
#endif

#ifdef CONFIG_driver_ccs811
	ccs811.read();
	kout << bin;
	kout << "CCS811 status / error: " << ccs811.status << " / " << ccs811.error_id << endl;
	kout << dec;
	kout << "CCS811 tVOC / eCO₂ : " << ccs811.tvoc << " ppb / " << ccs811.eco2 << " ppm" << endl;
#endif

#ifdef CONFIG_driver_ds2482
	unsigned char addr[8];
	ds2482.readROM(addr, 8);
	kout << hex << "DS2482 ROM address: ";
	for (unsigned char i = 0; i < 8; i++) {
		kout << (unsigned int)addr[i];
	}
	kout << " / ";
	for (signed char i = 7; i >= 0; i--) {
		kout << (unsigned int)addr[i];
	}
	kout << endl;
#endif

#ifdef CONFIG_driver_hdc1080
	/*
	hdc1080.heater(1);
	for (unsigned char i = 0; i < 50; i++) {
		hdc1080.getTemp();
	}
	*/
	hdc1080.measure();
	arch.delay_ms(10);
	kout << "HDC1080 temperature " << hdc1080.getTemp() << " degC" << endl;
	kout << "HDC1080 humidity " << hdc1080.getRH() << " %H" << endl;
#endif

#ifdef CONFIG_driver_mpu9250
	int mx, my, mz;
	kout << "Temperature: " << mpu9250.getTemperature() << endl;
	kout << "Accel X " << mpu9250.getAccelX() << endl;
	kout << "Accel Y " << mpu9250.getAccelY() << endl;
	kout << "Accel Z " << mpu9250.getAccelZ() << endl;
	kout << "Gyro X " << mpu9250.getGyroX() << endl;
	kout << "Gyro Y " << mpu9250.getGyroY() << endl;
	kout << "Gyro Z " << mpu9250.getGyroZ() << endl;
	mpu9250.getRawMagnet(&mx, &my, &mz);
	kout << "Magnet X " << mx << endl;
	kout << "Magnet Y " << my << endl;
	kout << "Magnet Z " << mz << endl;
#endif

#ifdef CONFIG_driver_max44009
	kout.printf_float(max44009.getLux());
	kout << endl;
#endif

#ifdef CONFIG_driver_tsl2591
	tsl2591.read();
	kout << dec << "TSL2591 CH0: " << tsl2591.ch0 << " / CH1: " << tsl2591.ch1;
	kout << hex << "   (status: 0x" << tsl2591.getStatus() << ")" << endl;
#endif

#ifdef CONFIG_driver_scd4x
	scd4x.read();
	kout << dec << "CO₂: " << scd4x.co2 << " ppm" << endl;
	kout << "Temperature: ";
	kout.printf_float(((175.0 * scd4x.rawTemperature) / 65536) - 45);
	kout << " °c" << endl;
	kout << "Humidity: ";
	kout.printf_float((100.0 * scd4x.rawHumidity) / 65536);
	kout << " %" << endl;
#endif
}

int main(void)
{

	arch.setup();
	gpio.setup();
	kout.setup();

#if defined(DRIVER_I2C) || defined(CONFIG_driver_softi2c)
	if (i2c.setup() != 0) {
		kout << "I2C setup FAILED" << endl;
		return 1;
	}
	kout << "I2C setup OK" << endl;
#endif

#ifdef CONFIG_driver_bme280
	bme280.intf = BME280_I2C_INTF;
	bme280.read = bme680_i2c_read;
	bme280.write = bme680_i2c_write;
	bme280.delay_ms = bme680_delay_ms;

	int8_t rslt = BME280_OK;
	rslt = bme280.init();
	kout << "BME280 init " << rslt << endl;

	bme280.settings.osr_p = BME280_OVERSAMPLING_16X;
	bme280.settings.osr_t = BME280_OVERSAMPLING_16X;
	bme280.settings.osr_h = BME280_OVERSAMPLING_16X;
	bme280.settings.filter = BME280_FILTER_COEFF_OFF;
	bme280.settings.standby_time = BME280_STANDBY_TIME_500_MS;
	bme280.setSensorSettings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL | BME280_STANDBY_SEL);
	bme280.enterNormalMode();
#endif

#ifdef CONFIG_driver_bme680
	bme680.intf = BME680_I2C_INTF;
	bme680.read = bme680_i2c_read;
	bme680.write = bme680_i2c_write;
	bme680.delay_ms = bme680_delay_ms;

	bme680.amb_temp = 25;

	int8_t rslt = BME680_OK;
	rslt = bme680.init();
	kout << "BME680 init " << rslt << endl;

	bme680.power_mode = BME680_FORCED_MODE;
	bme680.tph_sett.os_hum = BME680_OS_1X;
	bme680.tph_sett.os_pres = BME680_OS_16X;
	bme680.tph_sett.os_temp = BME680_OS_2X;

	bme680.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	bme680.gas_sett.heatr_dur = 100;
	bme680.gas_sett.heatr_temp = 300;
	bme680.setSensorSettings(BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL);
#endif

#ifdef CONFIG_driver_ccs811
	kout << hex;
	kout << "CCS811 HWID: " << ccs811.getManufacturerID() << endl;
	arch.delay_ms(65);
	kout << "CCS811 status: " << ccs811.getStatus() << endl;
	arch.delay_ms(65);
	ccs811.init();
	arch.delay_ms(65);
	kout << "CCS811 status: " << ccs811.getStatus() << endl;
	kout << dec;
	ccs811.setMode(1);
	arch.delay_ms(50);
#endif

#ifdef CONFIG_driver_ds2482
	ds2482.setup();
#endif

#ifdef CONFIG_driver_hdc1080
	hdc1080.init();
	if (hdc1080.getManufacturerID() != 0x5449) {
		kout << "[!] invalid HDC1080 manufacturer ID: " << hex << hdc1080.getManufacturerID() << endl;
	}
#endif

#ifdef CONFIG_driver_mpu9250
	mpu9250.init();
	mpu9250.nineAxis();
#endif

#ifdef CONFIG_driver_tsl2591
	tsl2591.init();
#endif

#ifdef CONFIG_driver_scd4x
	scd4x.start();
#endif

	arch.idle_loop();

	return 0;
}
