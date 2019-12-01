#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
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
	unsigned short i = 0;

	arch.setup();
	gpio.setup();
	kout.setup();

	if (i2c.setup() != 0) {
		kout << "I2C setup failed" << endl;
		return 1;
	}

	kout << "I2C setup OK" << endl;

	mpu9250.init();
	mpu9250.accelOnly();

	while (1) {
		mpu9250.getRawAccel(&ax, &ay, &az);

		UPDATE_MIN(min_ax, ax);
		UPDATE_MIN(min_ay, ay);
		UPDATE_MIN(min_az, az);
		UPDATE_MAX(max_ax, ax);
		UPDATE_MAX(max_ay, ay);
		UPDATE_MAX(max_az, az);

		if (i++ == 2000) {
			kout << "Min Accel: " << min_ax << " / " << min_ay << " / " << min_az << endl;
			kout << "Max Accel: " << max_ax << " / " << max_ay << " / " << max_az << endl;
			kout << "Temp: " << mpu9250.getTemperature() << endl;
			min_ax = min_ay = min_az = 30000;
			max_ax = max_ay = max_az = -30000;
			i = 0;
		}
		arch.delay_ms(1);
	}

	return 0;
}
