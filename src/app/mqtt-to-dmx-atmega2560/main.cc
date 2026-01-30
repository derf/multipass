/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/stdin.h"
#include "driver/stdout3.h"
#include "driver/stdin3.h"
#include "driver/uptime.h"

#if defined(CONFIG_meta_driver_dmx1)
#include "driver/dmx1.h"
#define dmx dmx1
#elif defined(CONFIG_meta_driver_dmx2)
#include "driver/dmx2.h"
#define dmx dmx2
#elif defined(CONFIG_meta_driver_dmx3)
#include "driver/dmx3.h"
#define dmx dmx3
#else
#error "No DMX driver found"
#endif

#include "driver/timer.h"

#include <string.h>

volatile unsigned char timer_done = 0;
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

	if (!strncmp(buffer, "RX:", 3)) {
		offset = 3;
	} else if (!strncmp(buffer, "> RX:", 5)) {
		offset = 5;
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
	kout3.setup();
	kin3.setup();

	for (unsigned char i = 0; i < dmx.num_frames; i++) {
		dmx.frames[i] = 0;
	}

	dmx.setup();
	timer.setup_hz(4);
	timer.start(1);

#if DEBUG
	kout << "Ready" << endl;
#endif

	while (1) {
		while (!timer_done) {
			arch.idle();
			while (kin3.hasKey()) {
				char key = kin3.getKey();
				if ((key >= ' ') && (key <= '~')) {
					buffer[buf_pos++] = key;
				}
#if DEBUG
				kout << key << flush;
#endif
				gpio.led_toggle(1);

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
		timer.stop();
		timer_done = 0;
		timer.start(1);

		//gpio.led_toggle(0);
		dmx.write();

		if (do_wdr++ == 200) {
			kout3 << "wdr()" << endl;
		}
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
