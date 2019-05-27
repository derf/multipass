#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#ifndef TIMER_CYCLES
#error makeflag timer_cycles=1 required
#endif

extern "C" {
	void asm_save_all();
	void asm_load_all();
	void asm_load_mem();
}

uint16_t i = 0;

class Transaction {
	public:
		inline Transaction() { asm_save_all(); }
		inline ~Transaction() {}
		inline void retry() { asm_load_all(); }
		inline void abort() { asm_load_mem(); }
};

void loop(void)
{
	gpio.led_toggle(1);
	{
		Transaction tx;
		kout << dec << i << endl;
		i++;
		if (!gpio.read(GPIO::p4_5)) {
			tx.abort();
		}
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();


	gpio.led_on(0);
	gpio.input(GPIO::p4_5, 1);

	asm_load_all();

	kout << "Hello, World!" << endl;

	arch.idle_loop();

	return 0;
}
