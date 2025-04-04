# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

COMMON_FLAGS = -g -Os -Wall -Wextra

# Only load .config when app/arch are not specified on commandline
ifneq (${app}, )
	CONFIG_app = ${app}
	CONFIG_arch = ${arch}
else
	-include .config
	COMMON_FLAGS += -include include/config.h
endif

# Make cannot include "foo"/Makefile.inc, so remove quotes from config entries
quote="
app_dir = $(subst ${quote},,${CONFIG_app})
arch_dir = $(subst ${quote},,${CONFIG_arch})
bsec_dir = $(subst ${quote},,${CONFIG_driver_bme680_bsec_path})

INCLUDES = -Iinclude -Iinclude/arch/${arch_dir}
CFLAGS = -std=c99
CXXFLAGS = -std=c++14 -fno-rtti -fno-threadsafe-statics -fno-exceptions

QUIET = @

ASM_TARGETS =
C_TARGETS =
CXX_TARGETS = src/app/${app_dir}/main.cc src/object/cpp_helpers.cc src/object/outputstream.cc
SO_TARGETS =

ifdef verbose
	QUIET =
endif

ifneq (${app_dir}, )
	include src/app/${app_dir}/Makefile.inc
endif

ifneq (${stack_usage}, )
	COMMON_FLAGS += -fstack-usage
endif

ifdef CONFIG_driver_dummy
	CXX_TARGETS += src/driver/dummy.cc
endif

ifdef CONFIG_driver_ads111x
	CXX_TARGETS += src/driver/ads111x.cc
endif

ifdef CONFIG_driver_lm75
	CXX_TARGETS += src/driver/lm75.cc
endif

ifdef CONFIG_driver_s5851a
	CXX_TARGETS += src/driver/s5851a.cc
endif

ifdef CONFIG_driver_am2320
	CXX_TARGETS += src/driver/am2320.cc
endif

ifdef CONFIG_driver_bme280
	CXX_TARGETS += src/driver/bme280.cc src/driver/bme680_util.cc
endif

ifdef CONFIG_driver_bme680
	CXX_TARGETS += src/driver/bme680.cc src/driver/bme680_util.cc
endif

ifdef CONFIG_driver_bme680_bsec
	SO_TARGETS += include/arch/${arch_dir}/driver/${bsec_dir}/libalgobsec.a
endif

ifdef CONFIG_driver_ccs811
	CXX_TARGETS += src/driver/ccs811.cc
endif

ifdef CONFIG_driver_ds2482
	CXX_TARGETS += src/driver/ds2482.cc
endif

ifdef CONFIG_driver_eeprom24lc64
	CXX_TARGETS += src/driver/eeprom24lc64.cc
endif

ifdef CONFIG_driver_max44006
	CXX_TARGETS += src/driver/max44006.cc
endif

ifdef CONFIG_driver_max44009
	CXX_TARGETS += src/driver/max44009.cc
endif

ifdef CONFIG_driver_hdc1080
	CXX_TARGETS += src/driver/hdc1080.cc
endif

ifdef CONFIG_driver_mpu9250
	CXX_TARGETS += src/driver/mpu9250.cc
endif

ifdef CONFIG_driver_mmsimple
	CXX_TARGETS += src/driver/mmsimple.cc
endif

ifdef CONFIG_driver_mmsubstate
	CXX_TARGETS += src/driver/mmsubstate.cc
endif

ifdef CONFIG_driver_tsl2591
	CXX_TARGETS += src/driver/tsl2591.cc
endif

ifdef CONFIG_driver_scd4x
	CXX_TARGETS += src/driver/scd4x.cc
endif

ifdef CONFIG_driver_sen5x
	CXX_TARGETS += src/driver/sen5x.cc
endif

ifdef CONFIG_driver_sen66
	CXX_TARGETS += src/driver/sen66.cc
endif

ifdef CONFIG_driver_veml6075
	CXX_TARGETS += src/driver/veml6075.cc
endif

ifdef CONFIG_driver_nrf24l01
	CXX_TARGETS += src/driver/nrf24l01.cc
	ifeq (${arch_dir}, msp430fr5994lp)
		nrf24l01_cs_pin ?= p1_3
		nrf24l01_en_pin ?= p6_2
		nrf24l01_irq_pin ?= p8_3
	endif
	ifeq (${arch_dir}, msp430fr5969lp)
		nrf24l01_en_pin ?= p4_3
		nrf24l01_cs_pin ?= p2_4
		nrf24l01_irq_pin ?= p1_5
	endif
	ifeq (${arch_dir}, arduino-nano)
		nrf24l01_en_pin ?= pc1
		nrf24l01_cs_pin ?= pc0
		nrf24l01_irq_pin ?= pc2
	endif
	COMMON_FLAGS += -DNRF24L01_EN_PIN=GPIO::${nrf24l01_en_pin}
	COMMON_FLAGS += -DNRF24L01_CS_PIN=GPIO::${nrf24l01_cs_pin}
	COMMON_FLAGS += -DNRF24L01_IRQ_PIN=GPIO::${nrf24l01_irq_pin}
endif

ifdef CONFIG_driver_pervasive_aurora_mb
	CXX_TARGETS += src/driver/pervasive_aurora_mb.cc
	ifeq (${arch_dir}, msp430fr5994lp)
		pervasive_aurora_mb_reset_pin ?= p7_0
		pervasive_aurora_mb_cs_pin ?= p5_7
		pervasive_aurora_mb_dc_pin ?= p7_1
		pervasive_aurora_mb_vcc_pin ?= p8_1
		pervasive_aurora_mb_busy_pin ?= p6_3
	endif
	COMMON_FLAGS += -DDRIVER_PERVASIVE_AURORA_MB
	COMMON_FLAGS += -DPERVASIVE_AURORA_RESET_PIN=GPIO::${pervasive_aurora_mb_reset_pin}
	COMMON_FLAGS += -DPERVASIVE_AURORA_CS_PIN=GPIO::${pervasive_aurora_mb_cs_pin}
	COMMON_FLAGS += -DPERVASIVE_AURORA_DC_PIN=GPIO::${pervasive_aurora_mb_dc_pin}
	COMMON_FLAGS += -DPERVASIVE_AURORA_VCC_PIN=GPIO::${pervasive_aurora_mb_vcc_pin}
	COMMON_FLAGS += -DPERVASIVE_AURORA_BUSY_PIN=GPIO::${pervasive_aurora_mb_busy_pin}
endif

ifdef CONFIG_driver_sharp96
	CXX_TARGETS += src/driver/sharp96.cc
	ifeq (${arch_dir}, msp430fr5969lp)
		sharp96_power_pin ?= p4_2
		sharp96_en_pin ?= p4_3
		sharp96_cs_pin ?= p2_4
	endif
	ifeq (${arch_dir}, msp430fr5994lp)
		sharp96_power_pin ?= p1_2
		sharp96_en_pin ?= p6_2
		sharp96_cs_pin ?= p1_3
	endif
	COMMON_FLAGS += -DDRIVER_SHARP6
	COMMON_FLAGS += -DSHARP96_POWER_PIN=GPIO::${sharp96_power_pin}
	COMMON_FLAGS += -DSHARP96_EN_PIN=GPIO::${sharp96_en_pin}
	COMMON_FLAGS += -DSHARP96_CS_PIN=GPIO::${sharp96_cs_pin}
endif

ifdef CONFIG_driver_ssd1306
	CXX_TARGETS += src/driver/ssd1306.cc
	COMMON_FLAGS += -DSSD1306_WIDTH=${CONFIG_driver_ssd1306_width} -DSSD1306_HEIGHT=${CONFIG_driver_ssd1306_height}
endif

ifdef CONFIG_driver_resistive_load
	CXX_TARGETS += src/driver/resistive_load.cc
	resistor1_pin ?= p3_0
	resistor2_pin ?= p3_1
	resistor3_pin ?= p3_2
	resistor4_pin ?= p3_3
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN1=GPIO::${resistor1_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN2=GPIO::${resistor2_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN3=GPIO::${resistor3_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN4=GPIO::${resistor4_pin}
endif

ifdef CONFIG_driver_aemr_transition_sync
	CXX_TARGETS += src/driver/aemr_transition_sync.cc
endif

ifdef CONFIG_driver_softi2c
	CXX_TARGETS += src/driver/soft_i2c.cc
endif

ifdef CONFIG_meta_driver_hardware_i2c
	COMMON_FLAGS += -DDRIVER_HARDWARE_I2C
endif

ifdef CONFIG_framebuffer
	CXX_TARGETS += src/object/framebuffer.cc
endif

ifdef CONFIG_lib_inflate
	CXX_TARGETS += src/lib/inflate.cc
endif

ifdef CONFIG_lib_inflate_checksum
	COMMON_FLAGS += -DDEFLATE_CHECKSUM
endif

ifdef CONFIG_lib_inflate_lut
	COMMON_FLAGS += -DDEFLATE_WITH_LUT
endif

# Configure drivers (TODO: Kconfig)

ifneq (${i2c_freq}, )
	CONFIG_i2c_freq = ${i2c_freq}
endif
ifdef CONFIG_i2c_freq
	COMMON_FLAGS += -DF_I2C=${CONFIG_i2c_freq}
endif

ifneq (${timer_freq}, )
	CONFIG_timer_freq = ${timer_freq}
endif
ifdef CONFIG_timer_freq
	COMMON_FLAGS += -DF_TIMER=${CONFIG_timer_freq}
endif

ifeq (${softi2c_timer}, 1)
	override arch_drivers += ,timer
	COMMON_FLAGS += -DSOFTI2C_TIMER
endif

# still used in makefiles
ifeq (${loop}, 1)
	COMMON_FLAGS += -DCONFIG_loop
endif

ifeq (${trace_malloc}, 1)
	CXX_TARGETS += src/lib/mpmalloc.cc
	COMMON_FLAGS += -DMULTIPASS_TRACE_MALLOC
endif

ifneq (${arch_dir}, )
	include src/arch/${arch_dir}/Makefile.inc
endif

default: build/system.elf

include/config.h: .config
	${QUIET}test -z "${app}" && awk -f script/conf2h.awk .config > include/config.h || : > include/config.h

stack: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

stackm: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl --machine-readable ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

Kconfig: script/mkconfig
	${QUIET}script/mkconfig > Kconfig

config: Kconfig
	${QUIET}kconfig-qconf Kconfig

nconfig: Kconfig
	${QUIET}kconfig-nconf Kconfig

randconfig: Kconfig
	${QUIET}script/randconfig

clean: arch_clean
	${QUIET}rm -f build/system.elf

help: arch_help
	@echo
	@echo "common flags:"
	@echo "    loop -- enable loop() function"
	@echo "    i2c_freq -- I2C Frequency in Hz"
	@echo "    timer_freq -- Timer frequency in Hz"
	@echo "    softi2c_timer -- Use hardware timer instead of busy waiting for I2C timing"
	@echo "    kout_nop -- Do not write output to stdout / serial console"
	@echo "    trace_malloc -- trace mpmalloc/mpcalloc/mprealloc calls on stdout"
	@echo "    stack_usage -- Generate .su files for stack usage estimation (-> make stack)"
	@echo
	@echo "${arch_dir} drivers:"
	@ls -1 src/arch/${arch_dir}/driver | fgrep .c | cut -d . -f 1 | sed 's/^/    /'
	@echo
	@echo "common drivers:"
	@ls -1 src/driver | fgrep .c | cut -d . -f 1 | sed 's/^/    /'
	@echo
	@echo "available architectures:"
	@ls -1 src/arch | sed 's/^/    /'

info: arch_info
	@echo "COMMON_FLAGS: ${COMMON_FLAGS}"
	@echo "CFLAGS: ${CFLAGS}"
	@echo "CXXFLAGS: ${CXXFLAGS}"
	@echo "Selected (common/arch) drivers: ${drivers} / ${arch_drivers}"

nfpkeys:
	@echo '{"OS Image": {"ROM": {"unit": "B", "description": "ROM size", "minimize": true}, "RAM": {"unit": "B", "description": "RAM size", "minimize": true}}}'

.PHONY: clean config default randconfig stack stackm help info nfpkeys
