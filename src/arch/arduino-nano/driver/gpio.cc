/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/gpio.h"
#include <avr/io.h>
#include <avr/interrupt.h>

GPIO gpio;

#ifndef __acweaving
ISR(PCINT0_vect)
{
}

ISR(PCINT1_vect)
{
}

/*
ISR(PCINT2_vect)
{
}
*/
#endif
