#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/timer.h"
#include "driver/uptime.h"

#ifndef F_TIMER
#define F_TIMER 100000
#endif

volatile unsigned char timer_done = 0;

inline void await_timer()
{
	timer_done = 0;
	timer.start(1);
	while (!timer_done) {
		arch.idle();
	}
	timer.stop();
}

void loop(void)
{
	gpio.led_toggle(1);
	kout << "start waiting" << endl;
	await_timer();
	kout << "done waiting" << endl;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
#if F_TIMER > 999
	timer.setup_khz(F_TIMER / 1000);
#else
	timer.setup_hz(F_TIMER);
#endif

	gpio.led_on(0);
	kout << "Timer set at " << dec << F_TIMER << " Hz" << endl;

	arch.idle_loop();

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
