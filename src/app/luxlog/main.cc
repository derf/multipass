#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/timer.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/max44009.h"

// Alle 10 Minuten ein Messwert -> 144 Werte pro Tag -> 28,4 Tage für 4096 Werte
// Alle 4 Minuten ein Messwert -> 360 Werte pro Tag -> 11,37 Tage für 4096 Werte
__attribute__ ((section(".text"))) uint32_t log[4096];

uint16_t log_offset = 0;
float val = 0;
uint8_t loop_count = 0;

void loop(void)
{
	if ((log_offset == 0) && (loop_count < 2)) {
		gpio.led_on(1);
		for (uint16_t i = 0; i < 4096; i++) {
			kout << i << " = " << log[i] << endl;
		}
		gpio.led_off(1);
	}

	if ( (loop_count % 10) == 0) {
		gpio.led_on(0);
		arch.sleep_ms(1);
		gpio.led_off(0);
	}

	if (++loop_count == 4*60) {
		loop_count = 0;
		val = max44009.getLux();

		log[log_offset] = val * 10;
		log[log_offset + 1] = 0x00C0FFEE;

		log_offset++;

		kout.printf_float(val);
		kout << endl;
	}
}

int main(void)
{

	arch.setup();
	gpio.setup();
	kout.setup();

	if (i2c.setup() != 0) {
		kout << "I2C setup failed" << endl;
		return 1;
	}

	kout << "I2C setup OK" << endl;

	arch.idle_loop();

	return 0;
}
