#include "arch.h"
#include <unistd.h>

#if defined(WITH_LOOP) || defined(TIMER_S)
#include "driver/uptime.h"
void loop();
#endif
#ifdef WITH_WAKEUP
void wakeup();
#endif

void Arch::setup(void) { }

void Arch::idle_loop(void)
{
	while (1) {
		sleep(1);
#ifdef WITH_LOOP
		loop();
#endif
#ifdef WITH_WAKEUP
		wakeup();
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}

void Arch::idle(void)
{
}

Arch arch;
