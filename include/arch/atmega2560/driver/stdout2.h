/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDOUTPUT2_H
#define STANDARDOUTPUT2_H

#include "object/outputstream.h"

class StandardOutput2 : public OutputStream {
	private:
		StandardOutput2(const StandardOutput2 &copy);

	public:
		StandardOutput2 () {}
		void setup();

		virtual void put(char c) override;
};

extern StandardOutput2 kout2;

#endif
