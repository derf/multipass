/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef MMSUBSTATE_H
#define MMSUBSTATE_H

class MicroMoodySubstate {
	private:
		MicroMoodySubstate(const MicroMoodySubstate &copy);

		unsigned char const address;
		unsigned char txbuf[3];

	public:
		MicroMoodySubstate(unsigned char const addr) : address(addr) {}

		void sleep();
		void noSubstates(unsigned char power1, unsigned char power2);
		void twoSubstates(unsigned char switchDutarion, unsigned char power);
		void fourSubstates(unsigned char switchDutarion, unsigned char power);
		void eightSubstates(unsigned char switchDutarion, unsigned char power);
		void setSubstates(unsigned char substateCount, unsigned char switchDuration, unsigned char power);
};

extern MicroMoodySubstate moody;

#endif
