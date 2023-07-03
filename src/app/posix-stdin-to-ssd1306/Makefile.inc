# vim:ft=make
#
# Copyright 2021 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,i2c
	CONFIG_driver_ssd1306 = y
	COMMON_FLAGS += -DCONFIG_driver_ssd1306
	CONFIG_driver_ssd1306_width = 128
	CONFIG_driver_ssd1306_height = 64
endif
