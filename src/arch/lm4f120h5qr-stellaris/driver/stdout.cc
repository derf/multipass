/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/stdout.h"

extern "C" {
#include "lm4f_cpp_wrapper.h"
};


void StandardOutput::setup()
{
	mp_uart_setup();
}

void StandardOutput::put(char c)
{
	mp_uart_send_blocking(c);
	if (c == '\n') {
		put('\r');
	}
}

StandardOutput kout;
