/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDINPUT_H
#define STANDARDINPUT_H

class StandardInput {
	private:
		StandardInput(const StandardInput &copy);
		static unsigned char const bufsize = 32;
		char buffer[bufsize];
		volatile unsigned char write_pos;
		unsigned char read_pos;

	public:
		StandardInput() : write_pos(0), read_pos(0) {}
		void setup();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos++] = key;
			write_pos %= bufsize;
		}
};

extern StandardInput kin;

#endif
