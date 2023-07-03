/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for Sharp LS013B4DN04 transflective LCD, also available as
 * "430BOOST-SHARP96" addon for the MSP430 LaunchPad series.
 */
#ifndef SHARP96_H
#define SHARP96_H

class Sharp96 {
	private:
		Sharp96(const Sharp96 &copy);
		unsigned char txbuf[2];
		unsigned char vcom;
		unsigned char swap_bits(unsigned char byte);

	public:
		Sharp96() : vcom(0) {}

		void setup();
		void powerOn();
		void powerOff();
		void clear();
		void writeLine(unsigned char line_no, unsigned char* data);
		void toggleVCOM();
};

extern Sharp96 sharp96;

#endif
