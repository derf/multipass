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
#ifdef DRIVER_EEPROM24LC64
#include "driver/eeprom24lc64.h"
#endif
#ifdef DRIVER_MAX44009
#include "driver/max44009.h"
#endif
#ifdef DRIVER_MMSIMPLE
#include "driver/mmsimple.h"
#endif

#ifndef I2CBENCH_SETOS
#error must define I2CBENCH_SETOS
#endif

void loop(void)
{
#ifdef DRIVER_LM75
	//kout.printf_float(lm75.getTemp());
	//kout << endl;
	lm75.setOS(I2CBENCH_SETOS);
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
#ifdef DRIVER_MAX44009
	kout.printf_float(max44009.getLux());
	kout << endl;
#endif
#ifdef DRIVER_EEPROM24LC64
	static unsigned short page = 0;
	eeprom24lc64.writePage(page, "Hello, World! Und so weiter, lol");
	kout << "Address " << page << endl;
	page = (page + 32) % (256*32);
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

#ifdef MULTIPASS_ARCH_msp430fr5969lp
	kout << "Device ID " << hex;
	kout << *(uint8_t*)0x1a0a << *(uint8_t*)0x1a0b;
	kout << *(uint8_t*)0x1a0c << *(uint8_t*)0x1a0d;
	kout << *(uint8_t*)0x1a0e << *(uint8_t*)0x1a0f;
	kout << *(uint8_t*)0x1a10 << *(uint8_t*)0x1a11;
	kout << endl;
#endif

	arch.idle_loop();

	return 0;
}
