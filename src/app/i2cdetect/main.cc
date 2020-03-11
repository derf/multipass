#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#ifdef DRIVER_LM75
#include "driver/lm75.h"
#endif
#ifdef DRIVER_S5851A
#include "driver/s5851a.h"
#endif
#ifdef DRIVER_AM2320
#include "driver/am2320.h"
#endif
#ifdef DRIVER_BME280
#include "driver/bme280.h"
#include "driver/bme680_util.h"
#endif
#ifdef DRIVER_BME680
#include "driver/bme680.h"
#include "driver/bme680_util.h"
#endif
#ifdef DRIVER_CCS811
#include "driver/ccs811.h"
#endif
#ifdef DRIVER_EEPROM24LC64
#include "driver/eeprom24lc64.h"
#endif
#ifdef DRIVER_MAX44009
#include "driver/max44009.h"
#endif
#ifdef DRIVER_HDC1080
#include "driver/hdc1080.h"
#endif
#ifdef DRIVER_MPU9250
#include "driver/mpu9250.h"
#endif
#ifdef DRIVER_MMSIMPLE
#include "driver/mmsimple.h"
#endif

void loop(void)
{
#ifdef DRIVER_LM75
	kout.printf_float(lm75.getTemp());
	kout << endl;
#endif
#ifdef DRIVER_S5851A
	kout.printf_float(s5851a.getTemp());
	kout << endl;
#endif
#ifdef DRIVER_AM2320
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
#ifdef DRIVER_BME280
	bme280.intf = BME280_I2C_INTF;
	bme280.read = bme680_i2c_read;
	bme280.write = bme680_i2c_write;
	bme280.delay_ms = bme680_delay_ms;

	int8_t rslt = BME280_OK;
	struct bme280_data comp_data;
	rslt = bme280.init();
	kout << "BME280 init " << rslt << endl;
	bme280.settings.osr_p = BME280_OVERSAMPLING_16X;
	bme280.settings.osr_t = BME280_OVERSAMPLING_16X;
	bme280.settings.osr_h = BME280_OVERSAMPLING_16X;
	bme280.settings.filter = BME280_FILTER_COEFF_OFF;
	bme280.settings.standby_time = BME280_STANDBY_TIME_500_MS;
	bme280.setSensorSettings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL | BME280_STANDBY_SEL);
	bme280.enterNormalMode();
	arch.delay_ms(100);
	rslt = bme280.getSensorData(BME280_ALL, &comp_data);
	kout << "BME280 read " << rslt << endl;
	kout << "BME280 temperature " << (float)comp_data.temperature / 100 << " degC" << endl;
	kout << "BME280 humidity " << (float)comp_data.humidity / 1024 << " %" << endl;
	kout << "BME280 pressure " << (float)comp_data.pressure / 100 << " Pa" << endl;
#endif
#ifdef DRIVER_BME680
	bme680.intf = BME680_I2C_INTF;
	bme680.read = bme680_i2c_read;
	bme680.write = bme680_i2c_write;
	bme680.delay_ms = bme680_delay_ms;
	/* amb_temp can be set to 25 prior to configuring the gas sensor 
	* or by performing a few temperature readings without operating the gas sensor.
	*/
	bme680.amb_temp = 25;

	int8_t rslt = BME680_OK;
	rslt = bme680.init();
	kout << "BME680 init " << rslt << endl;

	bme680.power_mode = BME680_FORCED_MODE;
	bme680.tph_sett.os_hum = BME680_OS_1X;
	bme680.tph_sett.os_pres = BME680_OS_16X;
	bme680.tph_sett.os_temp = BME680_OS_2X;

	bme680.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	bme680.gas_sett.heatr_dur = 30;
	bme680.gas_sett.heatr_temp = 300;
	bme680.setSensorSettings(BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL);
	bme680.setSensorMode();
	arch.delay_ms(200);
	struct bme680_field_data data;
	bme680.getSensorData(&data);
	kout << "BME680 temperature " << (float)data.temperature / 100 << " degC" << endl;
	kout << "BME680 humidity " << (float)data.humidity / 1000  << " %" << endl;
	kout << "BME680 pressure " << (float)data.pressure / 100 << " hPa" << endl;
	kout << "BME680 gas resistance " << data.gas_resistance << endl;
#endif
#ifdef DRIVER_CCS811
	kout << "CCS811 status is " << ccs811.check() << endl;
#endif
#ifdef DRIVER_HDC1080
	/*
	hdc1080.heater(1);
	for (unsigned char i = 0; i < 50; i++) {
		hdc1080.getTemp();
	}
	*/
	kout << "HDC1080 temperature " << hdc1080.getTemp() << " degC" << endl;
	kout << "HDC1080 humidity " << hdc1080.getRH() << " %H" << endl;
#endif
#ifdef DRIVER_MPU9250
	int mx, my, mz;
	kout << "Temperature: " << mpu9250.getTemperature() << endl;
	kout << "Accel X " << mpu9250.getAccelX() << endl;
	kout << "Accel Y " << mpu9250.getAccelX() << endl;
	kout << "Accel Z " << mpu9250.getAccelX() << endl;
	kout << "Gyro X " << mpu9250.getGyroX() << endl;
	kout << "Gyro Y " << mpu9250.getGyroX() << endl;
	kout << "Gyro Z " << mpu9250.getGyroX() << endl;
	mpu9250.getRawMagnet(&mx, &my, &mz);
	kout << "Magnet X " << mx << endl;
	kout << "Magnet Y " << my << endl;
	kout << "Magnet Z " << mz << endl;
#endif
#ifdef DRIVER_MAX44009
	kout.printf_float(max44009.getLux());
	kout << endl;
#endif
#ifdef DRIVER_EEPROM24LC64
	char buf[33];
	static unsigned char page = 0;
	eeprom24lc64.writePage(page, "Hello, World! Und so weiter, lol");
	arch.delay_ms(10);
	eeprom24lc64.readPage(page, buf);
	buf[32] = '\0';
	kout << "Address " << page << ": " << buf << endl;
	page++;
#endif
#ifdef DRIVER_MMSIMPLE
	moody.toggleBlue();
#endif
}

unsigned int i2c_status[128 / (8 * sizeof(unsigned int)) + 1];

int main(void)
{

	arch.setup();
	gpio.setup();
	kout.setup();

	if (i2c.setup() != 0) {
		return 1;
	}

	kout << "I2C setup OK" << endl;

#ifdef DRIVER_CCS811
	ccs811.init();
#endif
#ifdef DRIVER_HDC1080
	hdc1080.init();
	if (hdc1080.getManufacturerID() != 0x5449) {
		kout << "[!] invalid HDC1080 manufacturer ID: " << hex << hdc1080.getManufacturerID() << endl;
	}
#endif
#ifdef DRIVER_MPU9250
	mpu9250.init();
#endif

	for (unsigned char i = 0; i < sizeof(i2c_status)/sizeof(unsigned int); i++) {
		i2c_status[i] = 0;
	}

	i2c.scan(i2c_status);

	kout << "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f";
	for (unsigned char address = 0; address < 128; address++) {
		if ((address & 0x0f) == 0) {
			kout << endl;
			kout.printf_uint8(address);
			kout << ":";
		}

		if (i2c_status[address / (8 * sizeof(unsigned int))] & (1 << (address % (8 * sizeof(unsigned int))))) {
			kout << " ";
			kout.printf_uint8(address);
		} else {
			kout << " --";
		}
	}
	kout << endl;

	arch.idle_loop();

	return 0;
}
