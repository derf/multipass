# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override timer_s = 1
endif

ASM_TARGETS += src/app/transactiontest/util.S
