/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

class DMX3 {
	private:
		DMX3(const DMX3 &copy);

	public:
		static unsigned char const num_frames = 32;
		unsigned char frames[num_frames];

		DMX3() {}

		void setup();
		void write();
};

extern DMX3 dmx3;
