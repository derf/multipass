/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Driver for 4.2" Pervasive Aurora Mb (V230/V231) E-Paper displays with
 * internal Timing Controller (iTC) connected to a Pervasive EPD Extension
 * Board Gen 2 (EXT2)-02. Configured for the 300x400px 4.2" E-Paper variant;
 * register contents and LUTs for other resolutions and revisions may differ.
 */
#pragma once

class PervasiveAuroraMb {
	private:
		PervasiveAuroraMb(const PervasiveAuroraMb &copy);
		bool isBusy();
		void spiWrite(unsigned char reg, const unsigned char *txbuf, unsigned int length);

	public:
		PervasiveAuroraMb() {}

		void setup();
		void powerOn();
		void initialize(signed char temperature = 20);
		void sendImage(unsigned char *frame, unsigned int x = 0, unsigned int y = 0, unsigned int w = 0, unsigned int h = 0);
		void sendUpdate();
		void powerOff();
};

extern PervasiveAuroraMb pervasiveAuroraMb;
