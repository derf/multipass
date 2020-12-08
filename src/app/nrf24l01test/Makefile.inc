# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override loop = 1
	override timer_s = 1
	override drivers += ,nrf24l01
endif

ifeq (${tx}, 1)
	COMMON_FLAGS += -DNRF24L01TEST_TX
endif
