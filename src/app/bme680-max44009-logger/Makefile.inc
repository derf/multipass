# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,i2c
	CONFIG_driver_bme680 = y
	CONFIG_driver_max44009 = y
	COMMON_FLAGS += -DCONFIG_driver_bme680 -DCONFIG_driver_max44009
endif

ifdef CONFIG_arch_posix
	CXX_TARGETS += src/app/${app_dir}/posix.cc
else
	CXX_TARGETS += src/app/${app_dir}/generic.cc
	COMMON_FLAGS += -DBME680_FLOAT_POINT_COMPENSATION
endif

