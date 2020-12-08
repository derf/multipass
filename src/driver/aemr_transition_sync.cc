/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/aemr_transition_sync.h"
#include "driver/gpio.h"
#include "arch.h"

void AEMRTransitionSync::setup()
{
	gpio.led_off(0);
}

void AEMRTransitionSync::transBlip()
{
	gpio.led_on(1);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans10u()
{
	gpio.led_on(1);
	arch.delay_us(10);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans100u()
{
	gpio.led_on(1);
	arch.delay_us(100);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans1m()
{
	gpio.led_on(1);
	arch.delay_ms(1);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans2m()
{
	gpio.led_on(1);
	arch.delay_ms(2);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans5m()
{
	gpio.led_on(1);
	arch.delay_ms(5);
	gpio.led_off(1);
}

void AEMRTransitionSync::trans10m()
{
	gpio.led_on(1);
	arch.delay_ms(10);
	gpio.led_off(1);
}

AEMRTransitionSync aemrTransitionSync;
