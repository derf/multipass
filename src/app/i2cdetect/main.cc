#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/lm75.h"
//#include "driver/mmsimple.h"

void loop(void)
{
	kout.printf_float(lm75.getTemp());
	kout << endl;
	//moody.toggleBlue();
}

int main(void)
{
	unsigned int i2c_status[128 / (8 * sizeof(unsigned int)) + 1];

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
