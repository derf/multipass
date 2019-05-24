#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#ifndef TIMER_CYCLES
#error makeflag timer_cycles=1 required
#endif

void loop(void)
{
#if 0
	static unsigned char i = 0;

	switch (i) {
		case 0:
			gpio.led_on(0);
			gpio.led_off(0);
			break;
		case 1:
			gpio.led_on(0);
			arch.delay_us(1);
			gpio.led_off(0);
			break;
		case 2:
			gpio.led_on(0);
			arch.delay_us(2);
			gpio.led_off(0);
			break;
		case 3:
			gpio.led_on(1);
			gpio.led_off(1);
			break;
		case 4:
			gpio.led_on(1);
			arch.delay_us(1);
			gpio.led_off(1);
			break;
		case 5:
			gpio.led_on(1);
			arch.delay_us(2);
			gpio.led_off(1);
			break;
		case 6:
			gpio.led_on(0);
			break;
		case 7:
			gpio.led_off(0);
			break;
		case 8:
			gpio.led_on(1);
			break;
		case 9:
			gpio.led_off(1);
			break;
		default:
			break;
	}
	

	i = (i+1) % 10;
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;

	arch.idle_loop();

	return 0;
}
