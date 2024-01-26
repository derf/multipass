/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDINPUT3_H
#define STANDARDINPUT3_H

class StandardInput3 {
	private:
		StandardInput3(const StandardInput3 &copy);
		static unsigned char const bufsize = 16;
		char buffer[bufsize];
		unsigned char write_pos, read_pos;

	public:
		StandardInput3() : write_pos(0), read_pos(0) {}
		void setup();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos] = key;
			write_pos = (write_pos + 1) % bufsize;
		}
};

extern StandardInput3 kin3;

#endif
