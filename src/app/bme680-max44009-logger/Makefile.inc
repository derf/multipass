# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,i2c
	CONFIG_driver_bme680 = y
	CONFIG_driver_max44009 = y
	COMMON_FLAGS += -DCONFIG_driver_bme680 -DCONFIG_driver_max44009 -DCONFIG_meta_driver_i2c -DCONFIG_meta_driver_hardware_i2c
endif

ifdef CONFIG_arch_posix
	CXX_TARGETS += src/app/${app_dir}/posix.cc
else
	CXX_TARGETS += src/app/${app_dir}/generic.cc
	COMMON_FLAGS += -DBME680_FLOAT_POINT_COMPENSATION
endif

