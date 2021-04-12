# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += ,i2c
	CONFIG_meta_driver_hardware_i2c = y
endif
