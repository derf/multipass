# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override timer_s = 1
	override arch_drivers += ,spi
	CONFIG_driver_nrf24l01 = y
	COMMON_FLAGS += -DCONFIG_driver_nrf24l01
endif

ifeq (${tx}, 1)
	COMMON_FLAGS += -DNRF24L01TEST_TX
endif
