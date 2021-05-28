/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(CONFIG_driver_softi2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/ssd1306.h"

unsigned char const image[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 192, 224, 240, 240, 240, 248, 248, 248, 248, 248, 248, 248, 248, 248, 240, 240, 240, 224, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 184, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 254, 255, 255, 255, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 30, 63, 63, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 223, 3, 255, 255, 255, 255, 255, 246, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 7, 15, 31, 15, 15, 143, 255, 255, 255, 255, 255, 239, 159, 31, 31, 15, 7, 3, 3, 0, 0, 15, 127, 255, 255, 255, 127, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 192, 224, 240, 248, 252, 254, 135, 7, 7, 6, 142, 236, 252, 248, 152, 16, 48, 96, 96, 192, 192, 128, 128, 128, 128, 128, 128, 128, 128, 128, 192, 192, 128, 128, 128, 192, 224, 248, 252, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 248, 240, 224, 128, 128, 128, 129, 193, 129, 128, 128, 128, 128, 128, 128, 128, 128, 128, 192, 192, 96, 32, 48, 16, 216, 248, 252, 204, 142, 6, 7, 7, 142, 254, 252, 248, 240, 224, 192, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 243, 1, 1, 14, 255, 255, 255, 31, 6, 0, 0, 63, 255, 255, 63, 7, 0, 0, 0, 248, 254, 255, 255, 125, 1, 1, 1, 49, 253, 255, 255, 253, 1, 1, 19, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 191, 1, 1, 33, 253, 255, 255, 253, 49, 1, 1, 1, 125, 255, 255, 252, 248, 0, 0, 0, 7, 63, 255, 255, 63, 0, 0, 6, 31, 255, 255, 255, 30, 1, 1, 243, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 7, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 31, 31, 3, 0, 0, 0, 0, 0, 3, 63, 191, 129, 192, 224, 254, 255, 255, 255, 63, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 127, 255, 255, 255, 224, 224, 192, 131, 191, 127, 7, 0, 0, 0, 0, 0, 3, 63, 63, 0, 0, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 0, 7, 15, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 160, 240, 240, 240, 240, 224, 224, 240, 248, 248, 124, 62, 63, 31, 31, 15, 7, 7, 3, 1, 0, 192, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 192, 0, 1, 3, 7, 7, 15, 31, 31, 63, 62, 124, 124, 248, 240, 248, 248, 248, 240, 176, 176, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void loop(void)
{
	static unsigned char i = 0;
	gpio.led_toggle(1);
	ssd1306.setInvert((i % 20) >= 10);
	ssd1306.setContrast((i % 10) * 25);

	i++;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	i2c.setup();
	ssd1306.init();

	ssd1306.showImage((unsigned char*)image, sizeof(image));

	arch.idle_loop();

	return 0;
}
