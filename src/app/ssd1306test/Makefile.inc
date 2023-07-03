# vim:ft=make
#
# Copyright 2021 Birte Kristina Friesel
#
# SPDX-License-Identifier: CC0-1.0

ifdef app
	loop = 1
	override arch_drivers += ,i2c
	CONFIG_meta_driver_i2c = y
	CONFIG_meta_driver_hardware_i2c = y
	CONFIG_driver_ssd1306 = y
	CONFIG_driver_ssd1306_width = 128
	CONFIG_driver_ssd1306_height = 64
	CONFIG_framebuffer = y
	CONFIG_framebuffer_width = 128
	CONFIG_framebuffer_height = 64
	COMMON_FLAGS += -DCONFIG_driver_ssd1306 -DCONFIG_meta_driver_i2c -DCONFIG_meta_driver_hardware_i2c -DCONFIG_driver_ssd1306 -DCONFIG_driver_ssd1306_width=128 -DCONFIG_driver_ssd1306_height=64 -DCONFIG_framebuffer -DCONFIG_framebuffer_width=128 -DCONFIG_framebuffer_height=64
endif
