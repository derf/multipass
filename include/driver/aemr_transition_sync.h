/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef AEMR_TRANSITION_SYNC_H
#define AEMR_TRANSITION_SYNC_H

class AEMRTransitionSync {
	private:
		AEMRTransitionSync(const AEMRTransitionSync &copy);

	public:
		AEMRTransitionSync() {}
		void setup();
		void transBlip();
		void trans10u();
		void trans100u();
		void trans1m();
		void trans2m();
		void trans5m();
		void trans10m();
};

extern AEMRTransitionSync aemrTransitionSync;

#endif
