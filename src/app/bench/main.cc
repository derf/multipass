#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#ifndef TIMER_CYCLES
#error makeflag timer_cycles=1 required
#endif


/*
 * For ESP8266: Flash reads must be 4-Byte-aligned -> uint32_t array
 */

__attribute__ ((section(".text"))) uint32_t arr[4096];
//uint32_t arr[1024];
uint16_t frob;

inline uint16_t bench_read(uint32_t *arr)
{
	uint16_t ts_pre, ts_post;
	ts_pre = uptime.get_cycles();
	frob += *arr;
	ts_post = uptime.get_cycles();
	return ts_post - ts_pre;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	uint16_t ts1 = uptime.get_cycles();
	uint16_t ts2 = uptime.get_cycles();
	uint16_t ts3 = uptime.get_cycles();
	uint16_t ts4 = uptime.get_cycles();
	uint16_t i;

	//for (i = 0; i < 1024; i++) {
	//	arr[i] = 1;
	//}

	gpio.led_on(0);
	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;
	kout << dec << ts1 << endl;
	kout << dec << ts2 << endl;
	kout << dec << ts3 << endl;
	kout << dec << ts4 << endl;

	for (int i = 0; i < sizeof(arr) / sizeof(uint32_t); i++) {
		kout << i << ": " << bench_read(&arr[i]) << endl;
	}
	for (signed int i = (sizeof(arr) / sizeof(uint32_t)) - 1; i >= 0; i--) {
		kout << i << ": " << bench_read(&arr[i]) << endl;
	}
	kout << frob << endl;

	arch.idle_loop();

	return 0;
}
