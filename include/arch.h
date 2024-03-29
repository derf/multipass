/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef ARCH_H
#define ARCH_H

class Arch {
	private:
		Arch(const Arch &copy);

	public:
		Arch () {}
		void setup();
		void idle_loop();
		void idle();

		// Delay functions are not exact
		void delay_us(unsigned int const us);
		void delay_ms(unsigned int const ms);
		void sleep_ms(unsigned int const ms);
};

extern Arch arch;

#endif
