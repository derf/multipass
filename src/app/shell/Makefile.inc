# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override loop = 0
	override wakeup = 1
	override arch_drivers += ,stdin
endif
