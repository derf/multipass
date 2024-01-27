/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDOUTPUT1_H
#define STANDARDOUTPUT1_H

#include "object/outputstream.h"

class StandardOutput1 : public OutputStream {
	private:
		StandardOutput1(const StandardOutput1 &copy);

	public:
		StandardOutput1 () {}
		void setup();

		virtual void put(char c) override;
};

extern StandardOutput1 kout1;

#endif
