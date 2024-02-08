/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: CC0-1.0
 */

MEMORY
{
	rom (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 320K
}

INCLUDE ext/libopencm3/lib/cortex-m-generic.ld
