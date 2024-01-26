/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#if defined(CONFIG_meta_driver_stdin1)
#include "driver/stdout1.h"
#include "driver/stdin1.h"
#define koutx kout1
#define kin kin1
#elif defined(CONFIG_meta_driver_stdin2)
#include "driver/stdout2.h"
#include "driver/stdin2.h"
#define koutx kout2
#define kin kin2
#elif defined(CONFIG_meta_driver_stdin3)
#include "driver/stdout3.h"
#include "driver/stdin3.h"
#define koutx kout3
#define kin kin3
#else
#error "No stdin driver found"
#endif

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
volatile unsigned char timer_done = 0;
char buffer[24];
unsigned char buf_pos = 0;

void input_to_dmx(void)
{
	unsigned char num = 0;
	/*
	 * The first DMX frame is always 0.  However, incoming UART messages from
	 * the ESP8266 start with " " after the "RX:" part has been removed. So,
	 * the first actual message will end up in dmx.frames[1] as intended.
	 */
	unsigned char dmx_frame = 0;
	for (unsigned int i = 0; i < buf_pos; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9')) {
			num *= 10;
			num += buffer[i] - '0';
		}
		else {
			dmx.frames[dmx_frame++] = num;
			num = 0;
		}
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	koutx.setup();
	kin.setup();

	for (unsigned char i = 0; i < dmx.num_frames; i++) {
		dmx.frames[i] = 0;
	}

	dmx.setup();
	timer.setup_hz(15);
	timer.start(1);

	while (1) {
		while (!timer_done) {
			arch.idle();
			while (kin.hasKey() && (buf_pos < sizeof(buffer))) {
				char key = kin.getKey();
				kout << key << flush;

				/*
				 * Whoever decided that it was a good idea to prefix MQTT messages with "RX: " when outputting them on UART?
				 * Oh right, that was yesterday-me.
				 */
				if ((key == 'R') || (key == 'X') || (key == ':')) {
					continue;
				}

				buffer[buf_pos++] = key;

				if ((key == '\n') || (key == '\r')) {
					input_to_dmx();
					kout << endl << "DMX: " << dmx.frames[1] << " " << dmx.frames[2] << " " << dmx.frames[3] << " " << dmx.frames[4] << endl << "> ";
					buf_pos = 0;
				}
			}
		}
		timer.stop();
		timer_done = 0;
		timer.start(1);

		gpio.led_toggle(0);
		dmx.write();
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
