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

/*
 * fgrep 'PROGMEM frame' frames.cc | while read line; do echo $line | tr -cd , | wc -c; done | sort -n
 * set buffer size to maximum number + 1.
 */
unsigned char deflate_buf[160];
unsigned char img_buf[(SSD1306_WIDTH * SSD1306_HEIGHT / 8)];

int main(void)
{
	unsigned int i = 0, j = 0;
	unsigned char line, frame_len;
	unsigned char *frame;
	arch.setup();
	gpio.setup();
	kout.setup();
	i2c.setup();
	ssd1306.init();

	timer.setup_hz(frame_rate);
	timer_done = 0;
	timer.start(1);

	while (1) {
		for (i = 0; i < (sizeof(frames) / sizeof(frames[0])); i++) {

			frame = (unsigned char *)pgm_read_ptr(&frames[i]);
			frame_len = (unsigned char)pgm_read_byte(&frame[0]);

			for (j = 0; j < frame_len; j++) {
				deflate_buf[j] = (unsigned char)pgm_read_byte(&frame[j+1]);
			}
			inflate(deflate_buf, frame_len + 4, img_buf, sizeof(img_buf));

			while (!timer_done) {
				arch.idle();
			}
			timer.stop();
			timer_done = 0;
			timer.start(1);

			ssd1306.showImage(img_buf, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
		}
	}

	return 0;
}

ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
