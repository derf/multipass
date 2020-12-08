/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#define ON_TIMER_INTERRUPT_head ISR(TIMER0_COMPA_vect) {
#define ON_TIMER_INTERRUPT_tail }

class Timer {
	private:
		Timer(const Timer &copy);
		unsigned char prescaler;

	public:
		Timer() {}

		inline void setup_khz(uint16_t const frequency) {
			OCR0A = frequency ? 255 / frequency : 1;
			TCCR0A = _BV(WGM01);
			prescaler = _BV(CS01) | _BV(CS00);
		}
		inline void setup_hz(uint16_t const frequency) {
			OCR0A = frequency ? 255 / frequency : 1;
			TCCR0A = _BV(WGM01);
			prescaler = _BV(CS02) | _BV(CS00);
		}
		inline void start(unsigned char const interrupt) {
			TCNT0 = 0;
			TCCR0B = prescaler;
			if (interrupt) {
				TIMSK0 = _BV(OCIE0A);
			}
		}
		inline void stop() { TCCR0B = 0; TIMSK0 = 0; }
};

extern Timer timer;
