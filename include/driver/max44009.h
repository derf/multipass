/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MAX44009_H
#define MAX44009_H

class MAX44009 {
	private:
		MAX44009(const MAX44009 &copy);
		unsigned char const address;
		unsigned char txbuf[2];
		unsigned char rxbuf[2];

	public:
		MAX44009(unsigned char const addr) : address(addr) {}

		float getLux();
};

extern MAX44009 max44009;

#endif
