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
#ifdef DRIVER_AM2320
#include "driver/am2320.h"
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
#ifdef DRIVER_MMSIMPLE
#include "driver/mmsimple.h"
#endif

void loop(void)
{
#ifdef DRIVER_LM75
	kout.printf_float(lm75.getTemp());
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
#ifdef DRIVER_BME680
	struct bme680_dev gas_sensor;

	gas_sensor.dev_id = BME680_I2C_ADDR_SECONDARY;
	gas_sensor.intf = BME680_I2C_INTF;
	gas_sensor.read = bme680_i2c_read;
	gas_sensor.write = bme680_i2c_write;
	gas_sensor.delay_ms = bme680_delay_ms;
	/* amb_temp can be set to 25 prior to configuring the gas sensor 
	* or by performing a few temperature readings without operating the gas sensor.
	*/
	gas_sensor.amb_temp = 25;

	int8_t rslt = BME680_OK;
	rslt = bme680_init(&gas_sensor);
	kout << "BME680 init " << rslt << endl;

	gas_sensor.power_mode = BME680_FORCED_MODE;
	gas_sensor.tph_sett.os_hum = BME680_OS_1X;
	gas_sensor.tph_sett.os_pres = BME680_OS_16X;
	gas_sensor.tph_sett.os_temp = BME680_OS_2X;

	gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	gas_sensor.gas_sett.heatr_dur = 150;
	gas_sensor.gas_sett.heatr_temp = 300;
	bme680_set_sensor_settings(BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL, &gas_sensor);
	bme680_set_sensor_mode(&gas_sensor);
	arch.delay_ms(500);
	struct bme680_field_data data;
	bme680_get_sensor_data(&data, &gas_sensor);
	kout << "BME680 temperature " << (float)data.temperature / 100 << " degC" << endl;
	kout << "BME680 humidity " << (float)data.humidity / 1000  << " %" << endl;
	kout << "BME680 pressure " << (float)data.pressure / 100 << " hPa" << endl;
	kout << "BME680 gas resistance " << data.gas_resistance << endl;
#endif
#ifdef DRIVER_CCS811
	kout << "CCS811 status is " << ccs811.check() << endl;
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
