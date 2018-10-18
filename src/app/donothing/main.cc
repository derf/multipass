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

	arch.idle();

	return 0;
}
