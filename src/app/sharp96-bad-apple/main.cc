/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/spi.h"
#include "driver/sharp96.h"
#include "lib/inflate.h"

#include "driver/timer.h"
volatile unsigned char timer_done = 0;

#include "frames.cc"

__attribute__((section(".leaRAM"))) unsigned char img_buf[(72 * 96 / 8) * 3];

int main(void)
{
	unsigned int i = 0;
	unsigned char line;
	arch.setup();
	gpio.setup();
	kout.setup();
	spi.setup();
	sharp96.setup();
	sharp96.powerOn();
	sharp96.clear();

	timer.setup_hz(frame_rate);

	while (1) {
		for (i = 0; i < (sizeof(frames) / sizeof(frames[0])); i++) {

			timer_done = 0;
			timer.start(1);

			for (line = 0; line < 72; line++) {
				sharp96.writeLine(line, img_buf + (12 * 72 * 2) + (12 * line));
			}

			inflate(frames[i], sizeof(img_buf), img_buf, sizeof(img_buf));

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();

			timer_done = 0;
			timer.start(1);

			for (line = 0; line < 72; line++) {
				sharp96.writeLine(line, img_buf + (12 * line));
			}

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();

			timer_done = 0;
			timer.start(1);

			for (line = 0; line < 72; line++) {
				sharp96.writeLine(line, img_buf + (12 * 72) + (12 * line));
			}

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();

			if ((i%10)==0) {
				sharp96.toggleVCOM();
			}
		}
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
