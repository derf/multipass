# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += i2c,timer
	CONFIG_driver_ssd1306 = y
	COMMON_FLAGS += -DCONFIG_driver_ssd1306
	CONFIG_driver_ssd1306_width = 128
	CONFIG_driver_ssd1306_height = 64
	CONFIG_lib_inflate = y
	CONFIG_lib_inflate_lut = y
	CONFIG_arch_msp430fr5994lp_large_mode = y
endif
