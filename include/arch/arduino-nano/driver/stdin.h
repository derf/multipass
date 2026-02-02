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
		char buffer[32];
		unsigned char write_pos, read_pos;

	public:
		StandardInput() : write_pos(0), read_pos(0) {}
		void setup();
		void disable();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos] = key;
			write_pos = (write_pos + 1) % (sizeof(buffer) / sizeof(buffer[0]));
		}
};

extern StandardInput kin;

#endif
