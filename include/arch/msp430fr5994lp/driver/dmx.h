/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

class DMX {
	private:
		DMX(const DMX &copy);

	public:
		unsigned char frames[16];

		DMX() {}

		void setup();
		void write();
};

extern DMX dmx;
