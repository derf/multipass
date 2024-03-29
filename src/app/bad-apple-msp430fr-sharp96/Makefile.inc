# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += spi,timer
	CONFIG_driver_sharp96 = y
	COMMON_FLAGS += -DCONFIG_driver_sharp96
	CONFIG_lib_inflate = y
	CONFIG_lib_inflate_lut = y
	CONFIG_arch_msp430fr5994lp_large_mode = y
endif
