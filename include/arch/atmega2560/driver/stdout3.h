/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDOUTPUT3_H
#define STANDARDOUTPUT3_H

#include "object/outputstream.h"

class StandardOutput3 : public OutputStream {
	private:
		StandardOutput3(const StandardOutput3 &copy);

	public:
		StandardOutput3 () {}
		void setup();

		virtual void put(char c) override;
};

extern StandardOutput3 kout3;

#endif
