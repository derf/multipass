# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override timer_s = 0
	override loop = 0
	override arch_drivers += ,counter
	override CONFIG_lib_inflate = y
endif

ifdef deflate_nop
	COMMON_FLAGS += -DDEFLATE_NOP
	override undefine CONFIG_lib_inflate
endif

ifdef deflate_checksum
	override CONFIG_lib_inflate_checksum = y
endif
