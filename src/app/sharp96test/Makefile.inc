# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,spi
	CONFIG_driver_sharp96 = y
	COMMON_FLAGS += -DCONFIG_driver_sharp96
endif
