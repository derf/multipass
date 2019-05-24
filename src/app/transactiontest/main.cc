#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#ifndef TIMER_CYCLES
#error makeflag timer_cycles=1 required
#endif

extern "C" {
	void asm_save_toc();
	void asm_load_toc();
}

volatile bool __attribute__((section(".text"))) have_state = false;
uint16_t i = 0;

void restore_state()
{
	if (!have_state) {
		return;
	}
	asm_load_toc();
}

void save_state()
{
	asm_save_toc();
	have_state = true;
}

void loop(void)
{
	gpio.led_toggle(1);
	kout << dec << i << endl;
	i++;
	if (i == 5) {
		save_state();
	}
	if (i == 10) {
		restore_state();
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	restore_state();

	gpio.led_on(0);
	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;

	arch.idle_loop();

	return 0;
}
