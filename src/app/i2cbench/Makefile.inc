# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += ,i2c
endif

ifneq (${i2cbench_setos}, )
	COMMON_FLAGS += -DI2CBENCH_SETOS=${i2cbench_setos}
endif
