/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

class DMX1 {
	private:
		DMX1(const DMX1 &copy);

	public:
		static unsigned char const num_frames = 32;
		unsigned char frames[num_frames];

		DMX1() {}

		void setup();
		void write();
};

extern DMX1 dmx1;
