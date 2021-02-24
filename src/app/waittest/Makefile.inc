# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	CONFIG_loop = y
	override timer_s = 1
	override arch_drivers += ,timer
endif
