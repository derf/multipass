/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDINPUT1_H
#define STANDARDINPUT1_H

class StandardInput1 {
	private:
		StandardInput1(const StandardInput1 &copy);
		static unsigned char const bufsize = 32;
		char buffer[bufsize];
		volatile unsigned char write_pos;
		unsigned char read_pos;

	public:
		StandardInput1() : write_pos(0), read_pos(0) {}
		void setup();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos++] = key;
			write_pos %= bufsize;
		}
};

extern StandardInput1 kin1;

#endif
