/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/resistive_load.h"
#include "driver/gpio.h"
#include "arch.h"

#ifndef RESISTIVE_LOAD_PIN1
#error RESISTIVE_LOAD_PIN1 must be set
#endif

#ifndef RESISTIVE_LOAD_PIN2
#error RESISTIVE_LOAD_PIN2 must be set
#endif

#ifndef RESISTIVE_LOAD_PIN3
#error RESISTIVE_LOAD_PIN3 must be set
#endif

#ifndef RESISTIVE_LOAD_PIN4
#error RESISTIVE_LOAD_PIN4 must be set
#endif

void ResistiveLoad::setup()
{
	gpio.output(RESISTIVE_LOAD_PIN1, 0);
	gpio.output(RESISTIVE_LOAD_PIN2, 0);
	gpio.output(RESISTIVE_LOAD_PIN3, 0);
	gpio.output(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::switchToNone()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 0);
	gpio.write(RESISTIVE_LOAD_PIN2, 0);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::switchTo750()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 1);
	gpio.write(RESISTIVE_LOAD_PIN2, 1);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::switchTo1K0()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 1);
	gpio.write(RESISTIVE_LOAD_PIN2, 0);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::switchTo2K4()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 0);
	gpio.write(RESISTIVE_LOAD_PIN2, 1);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 1);
}

void ResistiveLoad::switchTo3K3()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 0);
	gpio.write(RESISTIVE_LOAD_PIN2, 1);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::switchTo10K()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 0);
	gpio.write(RESISTIVE_LOAD_PIN2, 0);
	gpio.write(RESISTIVE_LOAD_PIN3, 0);
	gpio.write(RESISTIVE_LOAD_PIN4, 1);
}

void ResistiveLoad::switchTo47K()
{
	gpio.write(RESISTIVE_LOAD_PIN1, 0);
	gpio.write(RESISTIVE_LOAD_PIN2, 0);
	gpio.write(RESISTIVE_LOAD_PIN3, 1);
	gpio.write(RESISTIVE_LOAD_PIN4, 0);
}

void ResistiveLoad::nop1K0(unsigned int duration_ms)
{
	switchTo1K0();
	arch.delay_ms(duration_ms);
	switchToNone();
}

void ResistiveLoad::nop2K4(unsigned int duration_ms)
{
	switchTo2K4();
	arch.delay_ms(duration_ms);
	switchToNone();
}

void ResistiveLoad::nop3K3(unsigned int duration_ms)
{
	switchTo3K3();
	arch.delay_ms(duration_ms);
	switchToNone();
}

void ResistiveLoad::nop10K(unsigned int duration_ms)
{
	switchTo10K();
	arch.delay_ms(duration_ms);
	switchToNone();
}

void ResistiveLoad::nop47K(unsigned int duration_ms)
{
	switchTo47K();
	arch.delay_ms(duration_ms);
	switchToNone();
}

ResistiveLoad resistiveLoad;
