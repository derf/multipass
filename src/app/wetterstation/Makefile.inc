# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,adc
endif
