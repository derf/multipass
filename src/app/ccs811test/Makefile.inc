# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += ,i2c
	override drivers += ,ccs811
	override loop = 1
endif
