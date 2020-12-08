/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"
#include <stdio.h>

void StandardOutput::put(char c)
{
	fputc(c, stdout);
}

void StandardOutput::flush()
{
	fflush(stdout);
}

StandardOutput kout;
