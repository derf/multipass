/*
 * Copyright 2022 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"
#include "sci.h"

void StandardOutput::setup()
{
	sciInit();
}

void StandardOutput::put(char c)
{
	while ((scilinREG->FLR & 0x04) == 0x04);
	sciSendByte(scilinREG, c);
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
