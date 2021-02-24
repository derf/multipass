# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override undefine CONFIG_loop
	CONFIG_wakeup = y
	override arch_drivers += ,stdin
endif
