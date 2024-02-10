/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/i2c.h"
#include "driver/ssd1306.h"
#include "lib/inflate.h"

#include "driver/timer.h"
volatile unsigned char timer_done = 0;

#include "frames.cc"

__attribute__((section(".leaRAM"))) unsigned char img_buf[(SSD1306_WIDTH * SSD1306_HEIGHT / 8) * 3];

int main(void)
{
	unsigned int i = 0;
	unsigned char line;
	arch.setup();
	gpio.setup();
	kout.setup();
	i2c.setup();
	ssd1306.init();

	timer.setup_hz_low(frame_rate);

	while (1) {
		for (i = 0; i < (sizeof(frames) / sizeof(frames[0])); i++) {

			timer_done = 0;
			timer.start(1);

			ssd1306.showImage(img_buf + (SSD1306_WIDTH * SSD1306_HEIGHT / 8 * 2), SSD1306_WIDTH * SSD1306_HEIGHT / 8);

			inflate(frames[i], sizeof(img_buf), img_buf, sizeof(img_buf));

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();

			timer_done = 0;
			timer.start(1);

			ssd1306.showImage(img_buf + (SSD1306_WIDTH * SSD1306_HEIGHT / 8 * 0), SSD1306_WIDTH * SSD1306_HEIGHT / 8);

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();

			timer_done = 0;
			timer.start(1);

			ssd1306.showImage(img_buf + (SSD1306_WIDTH * SSD1306_HEIGHT / 8 * 1), SSD1306_WIDTH * SSD1306_HEIGHT / 8);

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
