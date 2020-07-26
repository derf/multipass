#include "arch.h"

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
}

#ifdef WITH_WAKEUP
extern void wakeup();
#endif

#if defined(WITH_LOOP)
extern void loop();
volatile char run_loop = 0;
#endif

void Arch::delay_us(unsigned int const us)
{
	volatile int x = us * 2;
	while (x--) {
		__asm("nop");
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		volatile int x = 2000;
		while (x--) {
			__asm("nop");
		}
	}
}

void Arch::idle_loop(void)
{
	while (1) {
		delay_ms(1000);
#ifdef WITH_LOOP
		loop();
#endif
	}
}

void Arch::idle(void)
{
#ifdef WITH_WAKEUP
	wakeup();
#endif
}

Arch arch;
