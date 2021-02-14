# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override loop = 1
	override arch_drivers += ,i2c
	override drivers += ,bme680,max44009
endif

COMMON_FLAGS += -DBME680_FLOAT_POINT_COMPENSATION
