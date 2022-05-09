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

void loop(void)
{
	counter.start();
	counter.stop();
	kout << "nop @ " << counter.value << "/" << counter.overflow << " cycles" << endl << endl;

	run_benchmark();
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	arch.idle_loop();

	return 0;
}
