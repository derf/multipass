# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override undefine loop
	wakeup = 1
	override arch_drivers += ,stdin
endif
