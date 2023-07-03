/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/timed_resistive_load.h"
#include "driver/gpio.h"
#include "arch.h"

#ifndef TIMED_RESISTIVE_LOAD_PIN1
#error TIMED_RESISTIVE_LOAD_PIN1 must be set
#endif

#ifndef TIMED_RESISTIVE_LOAD_PIN2
#error TIMED_RESISTIVE_LOAD_PIN2 must be set
#endif

#ifndef TIMED_RESISTIVE_LOAD_PIN3
#error TIMED_RESISTIVE_LOAD_PIN3 must be set
#endif

#ifndef TIMED_RESISTIVE_LOAD_PIN4
#error TIMED_RESISTIVE_LOAD_PIN4 must be set
#endif

void TimedResistiveLoad::setup()
{
	gpio.output(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.output(TIMED_RESISTIVE_LOAD_PIN2, 0);
	gpio.output(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.output(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

void TimedResistiveLoad::switchToNone()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

void TimedResistiveLoad::switchTo750()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

void TimedResistiveLoad::switchTo1K0()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

void TimedResistiveLoad::switchTo2K4()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 1);
}

void TimedResistiveLoad::switchTo3K3()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

void TimedResistiveLoad::switchTo10K()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 1);
}

void TimedResistiveLoad::switchTo47K()
{
	gpio.write(TIMED_RESISTIVE_LOAD_PIN1, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN2, 0);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN3, 1);
	gpio.write(TIMED_RESISTIVE_LOAD_PIN4, 0);
}

TimedResistiveLoad timedResistiveLoad;
