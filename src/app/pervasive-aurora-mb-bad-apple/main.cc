/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/spi.h"
#include "driver/pervasive_aurora_mb.h"
#include "object/framebuffer.h"
#include "lib/inflate.h"

#include "driver/timer.h"
volatile unsigned char timer_done = 0;

#include "frames.cc"

__attribute__((section(".leaRAM"))) unsigned char img_buf[160 * 120 / 8];

/*
 * Flashless Update isn't working properly yet. Black-to-white transitions
 * appear to be pretty much broken (or rather, nearly invisible).
 * Or I broke my display, who knows.
 */

int main(void)
{
	unsigned int i = 0;
	unsigned char line;
	arch.setup();
	gpio.setup();
	kout.setup();
	spi.setup();
	pervasiveAuroraMb.setup();

	timer.setup_hz_low(frame_rate);

	while (1) {
		for (i = 0; i < (sizeof(frames) / sizeof(frames[0])); i++) {

			timer_done = 0;
			timer.start(1);

			inflate(frames[i], sizeof(img_buf), img_buf, sizeof(img_buf));
			fb.clear();
			fb.drawAt(0, 0, 160, 120, img_buf);

			pervasiveAuroraMb.powerOn();
			pervasiveAuroraMb.initialize(20, i%20);
			pervasiveAuroraMb.sendImage((unsigned char*)fb.data, 0, 0, 160, 120);
			pervasiveAuroraMb.sendUpdate();
			pervasiveAuroraMb.powerOff();

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();
		}
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
