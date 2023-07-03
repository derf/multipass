/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/counter.h"

void loop(void)
{
	counter.start();
	counter.stop();
	kout << "nop: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_us(10);
	counter.stop();
	kout << "10us: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_us(20);
	counter.stop();
	kout << "20us: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(1);
	counter.stop();
	kout << "1ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(2);
	counter.stop();
	kout << "2ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(4);
	counter.stop();
	kout << "4ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(8);
	counter.stop();
	kout << "8ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(16);
	counter.stop();
	kout << "16ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;

	counter.start();
	arch.delay_ms(32);
	counter.stop();
	kout << "32ms: " << counter.value << "/" << counter.overflow << " cycles" << endl;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	arch.idle_loop();
	return 0;
}
