#include "arch.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void Arch::setup(void)
{
#ifdef TIMER_CYCLES
	TCCR0A = 0;
	TCCR0B = _BV(CS00);
#endif

#if defined(WITH_LOOP) || defined(TIMER_S)
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS12) | _BV(CS10); // /1024
	OCR1A = 15625;
	TIMSK1 = _BV(OCIE1A);
#endif

#ifdef TIMER_US
	// 16MHz/8 -> 2MHz timer
	TCCR2A = 0;
	TCCR2B = _BV(CS21);
#endif
	sei();
}

#if defined(WITH_LOOP) || defined(TIMER_S)

#include "driver/uptime.h"
extern void loop();

#endif

void Arch::idle_loop(void)
{
	while (1) {
		SMCR = _BV(SE);
		asm("sleep");
		SMCR = 0;
		asm("wdr");
#ifdef WITH_LOOP
		loop();
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}

Arch arch;

#if defined(WITH_LOOP) || defined(TIMER_S)

ISR(TIMER1_COMPA_vect)
{
}

#endif
