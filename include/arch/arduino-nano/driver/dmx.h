/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

class DMX {
	private:
		DMX(const DMX &copy);

	public:
		static unsigned char const num_frames = 16;
		unsigned char frames[num_frames];

		DMX() {}

		void setup();
		void write();
};

extern DMX dmx;
