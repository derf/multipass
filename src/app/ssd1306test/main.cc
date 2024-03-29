/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#elif defined(CONFIG_driver_softi2c)
#include "driver/soft_i2c.h"
#endif

#include "driver/ssd1306.h"
#include "object/framebuffer.h"
#include "lib/pixelfont/pixeloperator_mirrored.h"

void loop(void)
{
	static unsigned char i = 0;
	fb << i << " ";
	ssd1306.showImage(fb.data, fb.width * fb.height / 8);
	i++;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	i2c.setup();
	ssd1306.init();

	fb.clear();
	fb.setFont(pixeloperator_mirrored);
	ssd1306.showImage(fb.data, fb.width * fb.height / 8);

	arch.idle_loop();

	return 0;
}
