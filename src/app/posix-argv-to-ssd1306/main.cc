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
#include "lib/pixelfont/terminus16.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	arch.setup();
	kout.setup();
	i2c.setup();

	fb.clear();
	fb.setFont(pixeloperator_mirrored);
	if (argc > 1) {
		for (unsigned char i = 1; i < argc && i < 13; i++) {
			if (i == 9) {
				fb.setFont(terminus16, 2);
				fb.setPos(0, 0);
			}
			fb << argv[i] << endl;
		}
	} else {
		ssd1306.init();
	}
	if (argc > 14) {
		fb.drawBattery(114, 0, atoi(argv[13]), atoi(argv[14]));
	}
	ssd1306.showImage(fb.data, fb.width * fb.height / 8);

	return 0;
}
