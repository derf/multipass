/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: CC0-1.0
 */
MEMORY {
	rom  (rx) : ORIGIN = 0x00000000, LENGTH = 256K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE cortex-m-generic.ld
