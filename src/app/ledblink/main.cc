#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

void loop(void)
{
	gpio.led_toggle(1);
#ifdef TIMER_S
	kout << dec << uptime.get_s() << endl;
#else
	kout << "beep boop" << endl;
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.led_on(0);
	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;

	arch.idle_loop();

	return 0;
}
