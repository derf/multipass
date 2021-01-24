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

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	while (1) {
		counter.start();
#ifdef DEFLATE_NOP
		kout << (unsigned char*)inflate_input << endl;
		uint16_t ret = 1;
#else
		int16_t ret = inflate_zlib((unsigned char*)inflate_input, inflate_input_size, deflate_output, deflate_output_size);
#endif
		counter.stop();
		kout << "inflate returned " << ret << endl;
		kout << "Output: " << (char*)deflate_output << endl;
		kout << "took " << counter.value << "/" << counter.overflow << " cycles" << endl;
	}

	arch.idle();

	return 0;
}
