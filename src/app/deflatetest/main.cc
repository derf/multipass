/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#include "driver/counter.h"

#include "lib/inflate.h"
#include "deflate_data.cc"

unsigned char deflate_output[1024];

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	for (uint8_t i = 0; i < 5; i++) {
		counter.start();
		int16_t ret = inflate_zlib((unsigned char*)inflate_input, inflate_input_size, deflate_output, sizeof(deflate_output));
		counter.stop();
		kout << "inflate returned " << ret << endl;
		kout << "Output: " << (char*)deflate_output << endl;
		kout << "took " << counter.value << "/" << counter.overflow << " cycles" << endl;
	}

	arch.idle();

	return 0;
}
