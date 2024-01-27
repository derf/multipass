/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

class DMX2 {
	private:
		DMX2(const DMX2 &copy);

	public:
		static unsigned char const num_frames = 32;
		unsigned char frames[num_frames];

		DMX2() {}

		void setup();
		void write();
};

extern DMX2 dmx2;
