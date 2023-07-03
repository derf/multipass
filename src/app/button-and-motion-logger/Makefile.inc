# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	override arch_drivers += ,adc,i2c
	CONFIG_driver_mpu9250 = y
	CONFIG_meta_driver_i2c = y
	CONFIG_meta_driver_hardware_i2c = y
	COMMON_FLAGS += -DCONFIG_driver_mpu9250 -DCONFIG_meta_driver_i2c -DCONFIG_meta_driver_hardware_i2c
endif
