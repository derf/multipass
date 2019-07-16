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

char buf[32];

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

#ifdef MULTIPASS_ARCH_msp430fr5969lp
	kout << "write: ";
	//kout << nrf24l01.write("foo", 3, true, true) << " ";
	//kout << nrf24l01.write("123456789", 10, true, true) << " ";
	kout << nrf24l01.write("123456789123456789", 20, true, true) << endl;
#else
	kout << "carrier " << nrf24l01.testCarrier() << " / RPD " << nrf24l01.testRPD();
	if (nrf24l01.available()) {
		nrf24l01.read(buf, 32);
		kout << " / data = " << buf << endl;
	} else {
		kout << " / no data" << endl;
	}
	nrf24l01.stopListening();
	nrf24l01.startListening();
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "nrf24l01.setup() ...";
	nrf24l01.setup();
	kout << " OK" << endl;

	kout << "nrf24l01 configure ...";
    unsigned char addr[5] = {0, 'D', 'E', 'R', 'F'};
	//nrf24l01.setAutoAck(1);
	//nrf24l01.enableAckPayload();
	nrf24l01.setDynamicPayloads(true);
	nrf24l01.setPALevel(Nrf24l01::RF24_PA_MAX);
	nrf24l01.setChannel(25);
	nrf24l01.setDataRate(Nrf24l01::RF24_2MBPS);
#ifdef MULTIPASS_ARCH_msp430fr5994lp
	nrf24l01.openReadingPipe(1, addr);
	nrf24l01.startListening();
#else
	nrf24l01.openWritingPipe((const uint8_t*)addr);
#endif
	kout << " OK" << endl;

	gpio.led_on(0);
	arch.idle_loop();

	return 0;
}
