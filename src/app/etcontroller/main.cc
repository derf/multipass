#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "Hello, World!" << endl;
	kout << "Naptime." << endl;

#ifdef CONFIG_ETCONTROLLER_NRST
	gpio.output(CONFIG_ETCONTROLLER_RESET_PIN, 0);
	arch.delay_ms(1);
	gpio.write(CONFIG_ETCONTROLLER_RESET_PIN, 1);
#else
	gpio.output(CONFIG_ETCONTROLLER_RESET_PIN, 1);
	arch.delay_ms(1);
	gpio.write(CONFIG_ETCONTROLLER_RESET_PIN, 0);
#endif

	arch.idle();

	return 0;
}
