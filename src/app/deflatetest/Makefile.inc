# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override timer_s = 0
	override loop = 0
	override arch_drivers += ,counter
endif

ifdef deflate_nop
	COMMON_FLAGS += -DDEFLATE_NOP
else
	CXX_TARGETS += src/lib/inflate.cc
endif
