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

#include "tree.c.inc"

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	counter.start();
	counter.stop();
	kout << "nop @ " << counter.value << "/" << counter.overflow << " cycles" << endl << endl;

	run_benchmark();

	while (1) {
		arch.idle();
	}

	return 0;
}
