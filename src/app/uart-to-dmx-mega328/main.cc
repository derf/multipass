/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/stdin.h"
#include "driver/dmx.h"
#include "driver/timer.h"

#include <string.h>

volatile unsigned char timer_running = 16;
char buffer[64];
unsigned char buf_pos = 0;
unsigned char do_wdr = 0;

bool input_to_dmx(void)
{
	unsigned char num = 0;
	unsigned char dmx_frame = 1;
	unsigned int offset = 0;

#if DEBUG
	kout << "parsing: " << buffer << endl;
#endif

	if (!strncmp(buffer, "TX:", 3)) {
		offset = 3;
	} else {
		return false;
	}

	for (unsigned int i = offset; (i < buf_pos) && buffer[i] ; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9')) {
			num *= 10;
			num += buffer[i] - '0';
		}
		else {
			dmx.frames[dmx_frame++] = num;
			num = 0;
		}
	}
	dmx.frames[dmx_frame++] = num;
	return true;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	kin.setup();

	for (unsigned char i = 0; i < dmx.num_frames; i++) {
		dmx.frames[i] = i*5;
	}

	/*
	 * lowest supported frequency: 62 Hz
	 * timer_running = 16 → actual frequency 4 Hz
	 */
	timer.setup_hz(64);
	timer.start(1);

#if DEBUG
	kout << "Ready" << endl;
#endif

	while (1) {
		while (timer_running) {
			arch.idle();
			while (kin.hasKey()) {
				char key = kin.getKey();
				if ((key >= ' ') && (key <= '~')) {
					buffer[buf_pos++] = key;
				}
#if DEBUG
				kout << key << flush;
#endif

				if (buf_pos >= sizeof(buffer)) {
					buf_pos = 0;
				}

				if (buf_pos && ((key == '\n') || (key == '\r'))) {
					buffer[buf_pos] = 0;
					if (input_to_dmx()) {
#if DEBUG
						kout << endl << "DMX: ";
						for (unsigned char i = 1; i < dmx.num_frames; i++) {
							kout << dmx.frames[i] << " ";
						}
						kout << endl << "> " << flush;
#endif
					}
					buf_pos = 0;
				}
			}
		}
		gpio.led_toggle(1);
		timer.stop();
		timer_running = 16;
		timer.start(1);

		kin.disable();
		dmx.setup();
		dmx.write();
		kout.setup();
		kin.setup();
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	if (timer_running) {
		timer_running--;
	}
ON_TIMER_INTERRUPT_tail
