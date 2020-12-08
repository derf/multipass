/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef RESISTIVE_LOAD_H
#define RESISTIVE_LOAD_H

/*
 * Resistance at 25°c
 * R1: 986R
 * R2: 3K25
 * R3: 46K3
 * R4: 9K86
 */

class ResistiveLoad {
	private:
		ResistiveLoad(const ResistiveLoad &copy);

	public:
		ResistiveLoad() {}
		void setup();
		void switchToNone();
		void switchTo750(); // 576R (R1 || R2)
		void switchTo1K0(); // 986R (R1)
		void switchTo2K4(); // 2K44 (R2 || 4)
		void switchTo3K3(); // 3K25 (R2)
		void switchTo10K(); // 9K86 (R4)
		void switchTo47K(); // 46K3 (R3)
		void nop1K0(unsigned int duration_ms);
		void nop2K4(unsigned int duration_ms);
		void nop3K3(unsigned int duration_ms);
		void nop10K(unsigned int duration_ms);
		void nop47K(unsigned int duration_ms);
};

extern ResistiveLoad resistiveLoad;

#endif
