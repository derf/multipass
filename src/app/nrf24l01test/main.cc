#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/nrf24l01.h"
#include "driver/counter.h"

#define TIMEIT(index, functioncall) \
	counter.start(); \
	functioncall; \
	counter.stop(); \
	kout << endl << index << " :: " << dec << counter.value << "/" << counter.overflow << endl;

void loop(void)
{
	gpio.led_toggle(1);
	uint8_t status = nrf24l01.getStatus();

	kout << "status: " << hex << status;
	if (status & 0x40) {
		kout << " RX_DR";
	}
	if (status & 0x20) {
		kout << " TX_DS";
	}
	if (status & 0x10) {
		kout << " MAX_RT";
	}
	if ((status & 0x0e) == 0x0e) {
		kout << " RX_EMPTY";
	}
	if (status & 0x01) {
		kout << " TX_FULL";
	}
	kout << endl;

	kout << "write: ";
	nrf24l01.setRetries(0, 0);
	nrf24l01.enableDynamicPayloads(false);
	nrf24l01.enableDynamicAck(false);
	TIMEIT("blocking write(3)", nrf24l01.write("foo", 3, true, true));
	TIMEIT("blocking write(10)", nrf24l01.write("123456789", 10, true, true));
	TIMEIT("blocking write(20)", nrf24l01.write("123456789123456789", 20, true, true));
	//TIMEIT("blocking write(30)", nrf24l01.write("123456789123456789123456789", 30, true, true));
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
