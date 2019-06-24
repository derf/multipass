#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/nrf24l01.h"

void loop(void)
{
	gpio.led_toggle(1);
	kout << "status: " << hex << nrf24l01.getStatus() << endl;
	kout << "write: ";
	kout << nrf24l01.write("foo", 3, true) << endl;
	nrf24l01.startListening();
	arch.delay_ms(10);
	nrf24l01.stopListening();
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "nrf24l01.setup() ...";
	nrf24l01.setup();
	kout << " OK" << endl;

	gpio.led_on(0);
	arch.idle_loop();

	return 0;
}
