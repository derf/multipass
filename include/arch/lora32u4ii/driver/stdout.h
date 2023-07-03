/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDOUTPUT_H
#define STANDARDOUTPUT_H

#include <avr/pgmspace.h>
#include "object/outputstream.h"

class StandardOutput : public OutputStream {
	private:
		StandardOutput(const StandardOutput &copy);

	public:
		StandardOutput () {}
		void setup();

		virtual void put(char c) override;
		OutputStream & pprint(const char *text);
};

extern StandardOutput kout;

#endif
