/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/ssd1306.h"
#include "object/framebuffer.h"
#include "lib/pixelfont/pixeloperator_mirrored.h"

#include <stdio.h>

char buf[32];

void loop()
{
}

int main(void)
{
	arch.setup();
	kout.setup();
	i2c.setup();
	ssd1306.init();

	fb.clear();
	ssd1306.showImage(fb.data, fb.width * fb.height / 8);
	fb.setFont(pixeloperator_mirrored);

	while (fgets(buf, 32, stdin)) {
		fb << buf;
		ssd1306.showImage(fb.data, fb.width * fb.height / 8);
	}

	return 0;
}