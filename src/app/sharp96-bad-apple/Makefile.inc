# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += spi,timer
	override drivers += sharp96
	override CONFIG_lib_inflate = y
endif
