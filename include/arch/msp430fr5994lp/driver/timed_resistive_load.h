/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef TIMED_RESISTIVE_LOAD_H
#define TIMED_RESISTIVE_LOAD_H

/*
 * Resistance at 25°c
 * R1: 986R
 * R2: 3K25
 * R3: 46K3
 * R4: 9K86
 */

class TimedResistiveLoad {
	private:
		TimedResistiveLoad(const TimedResistiveLoad &copy);

	public:
		TimedResistiveLoad() {}
		void setup();
		void switchToNone();
		void switchTo750(); // 576R (R1 || R2)
		void switchTo1K0(); // 986R (R1)
		void switchTo2K4(); // 2K44 (R2 || 4)
		void switchTo3K3(); // 3K25 (R2)
		void switchTo10K(); // 9K86 (R4)
		void switchTo47K(); // 46K3 (R3)

		/*
		 * These functions must be inline, as __delay_cycles only works with
		 * compile-time constants. So they must be part of the compilation unit
		 * which uses them and cannot easily be wrapped by a function without
		 * losing accuracy.
		 */

		inline void __attribute__((always_inline)) nop1K0(unsigned long long int const duration_us)
		{
			switchTo1K0();
			__delay_cycles(F_CPU / 1000000UL * duration_us);
			switchToNone();
		}

		inline void __attribute__((always_inline)) nop2K4(unsigned long long int const duration_us)
		{
			switchTo2K4();
			__delay_cycles(F_CPU / 1000000UL * duration_us);
			switchToNone();
		}

		inline void __attribute__((always_inline)) nop3K3(unsigned long long int const duration_us)
		{
			switchTo3K3();
			__delay_cycles(F_CPU / 1000000UL * duration_us);
			switchToNone();
		}

		inline void __attribute__((always_inline)) nop10K(unsigned long long int const duration_us)
		{
			switchTo10K();
			__delay_cycles(F_CPU / 1000000UL * duration_us);
			switchToNone();
		}

		inline void __attribute__((always_inline)) nop47K(unsigned long long int const duration_us)
		{
			switchTo47K();
			__delay_cycles(F_CPU / 1000000UL * duration_us);
			switchToNone();
		}
};

extern TimedResistiveLoad timedResistiveLoad;

#endif
