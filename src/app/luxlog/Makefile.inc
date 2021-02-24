# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	CONFIG_loop = y
	override arch_drivers += ,i2c
	CONFIG_driver_max44009 = y
endif
