/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"
#include <machine/wdtcon.h>
#include <tc1796b/port5-struct.h>
#include <tc1796b/asc0-struct.h>

#define OF_BYP 29
#define OF_NDIV 16
#define OF_PDIV 13
#define OF_KDIV 8
#define OF_VCOSEL 6
#define OF_SYSFS 2

#define OF_RMC 8
#define ASC0_CLC (*(volatile unsigned int*)0xf0000a00)
#define ASC0_PISEL (*(volatile unsigned int*)0xf0000a04)
#define ASC0_CON (*(volatile unsigned int*)0xf0000a10)
#define ASC0_BG (*(volatile unsigned int*)0xf0000a14)
#define ASC0_FDV (*(volatile unsigned int*)0xf0000a18)
#define ASC0_TBUF (*(volatile unsigned int*)0xf0000a20)
#define ASC0_TBSRC (*(volatile unsigned int*)0xf0000afc)

#define SRC_SRE (1 << 12)
#define SRC_SRR (1 << 13)
#define SRC_CLRR (1 << 14)
#define SRC_SETR (1 << 15)

void StandardOutput::setup()
{
	// P5_IOCR0.PC1 = OUT_PPALT1
	(*(volatile unsigned int*)0xf0001110) = 0x9 << 12;
	// P5_IOMR.PS1 = 1
	(*(volatile unsigned int*)0xf0001104) = 0x00000001;

	/* Configure for 115200 Baud @ 75 MHz fSYS, see table 19-4 */
	unsigned int const reload_value = 0x17;
	unsigned int const fdv = 0x12e;

	unlock_wdtcon();
	// ASC0_CLC: enable (DISR := 0), RMC := 1  (clock divider == 1 -> clock == system clock "fsys"?)
	ASC0_CLC = 1 << OF_RMC;
	lock_wdtcon();
	// ASC0_CON = 0
	ASC0_CON = 0;
	// ASC0_BG = reload_value
	ASC0_BG = reload_value;
	// ASC0_FDV = fdv
	ASC0_FDV = fdv;
	// ASC0_CON := (M := ASCM_8ASYNC == 1; FDE := 1; R := 1
	ASC0_CON = (1 << 0) | (1 << 11) | (1 << 15);

	/* After initialization, the transmit buffer is ready to accept writes. */
	ASC0_TBSRC = SRC_SETR;
}

void StandardOutput::put(char c)
{
	while (!(ASC0_TBSRC & SRC_SRR)) ;

	/* Clear service request flag -- we're filling up TBUF */
	ASC0_TBSRC = SRC_CLRR;

	ASC0_TBUF = c;

	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
