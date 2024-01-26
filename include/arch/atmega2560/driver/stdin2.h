/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDINPUT2_H
#define STANDARDINPUT2_H

class StandardInput2 {
	private:
		StandardInput2(const StandardInput2 &copy);
		static unsigned char const bufsize = 16;
		char buffer[bufsize];
		unsigned char write_pos, read_pos;

	public:
		StandardInput2() : write_pos(0), read_pos(0) {}
		void setup();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos] = key;
			write_pos = (write_pos + 1) % bufsize;
		}
};

extern StandardInput2 kin2;

#endif
