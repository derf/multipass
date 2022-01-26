/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#define ON_TIMER_INTERRUPT_head ISR(TIMER4_COMPA_vect) {
#define ON_TIMER_INTERRUPT_tail }

class Timer {
	private:
		Timer(const Timer &copy);
		unsigned char prescaler;

	public:
		Timer() {}

		inline void setup_khz(uint16_t const frequency) { // 16 MHz base
			OCR4A = frequency ? 16000 / frequency : 65535;
			TCCR4A = 0;
			TCCR4B = _BV(WGM42);
			prescaler = _BV(CS40);
		}
		inline void setup_hz(uint16_t const frequency) { // 16 MHz / 256 == 62.5 kHz base
			OCR4A = frequency ? 62500 / frequency : 65535;
			TCCR4A = 0;
			TCCR4B = _BV(WGM42);
			prescaler = _BV(CS42);
		}
		inline void start(unsigned char const interrupt) {
			TCNT4 = 0;
			TCCR4B |= prescaler;
			if (interrupt) {
				TIMSK4 = _BV(OCIE4A);
			}
		}
		inline void stop() { TCCR4B = 0; TIMSK4 = 0; }
};

extern Timer timer;
