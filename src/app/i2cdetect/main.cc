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
