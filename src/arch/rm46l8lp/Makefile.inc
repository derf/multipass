# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

SERIAL_PORT ?= ttyACM0

cpu_freq ?= 160000000
counter_freq ?= 80000000

TI_CGT_PATH ?= /home/derf/lib/local/ti/ccs1200/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS
UNIFLASH_PATH ?= /home/derf/lib/local/opt/ti/uniflash_7.2.0

INCLUDES += --include_path=${TI_CGT_PATH}/include --include_path=include/arch/rm46l8lp/halcogen

# this is not gcc
COMMON_FLAGS =
CFLAGS =
CXXFLAGS =

COMMON_FLAGS += --preinclude=include/config.h
COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
COMMON_FLAGS += -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --opt_for_speed=0
COMMON_FLAGS += --define=_TMS570LS12x_ -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi
#CXXFLAGS += --c++14

CC = ${TI_CGT_PATH}/bin/armcl
CXX = ${CC}
SIZE = ${TI_CGT_PATH}/bin/armsize

ARCH_SHORTNAME = rm46l8

C_TARGETS += src/arch/rm46l8lp/halcogen/adc.c
C_TARGETS += src/arch/rm46l8lp/halcogen/errata_SSWF021_45.c
C_TARGETS += src/arch/rm46l8lp/halcogen/esm.c
C_TARGETS += src/arch/rm46l8lp/halcogen/gio.c
C_TARGETS += src/arch/rm46l8lp/halcogen/pinmux.c
C_TARGETS += src/arch/rm46l8lp/halcogen/notification.c
C_TARGETS += src/arch/rm46l8lp/halcogen/rti.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_dma.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_phantom.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sci.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_pcr.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_pmm.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_startup.c
C_TARGETS += src/arch/rm46l8lp/halcogen/system.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_vim.c
C_TARGETS += src/arch/rm46l8lp/halcogen/sys_selftest.c
CXX_TARGETS += src/arch/rm46l8lp/arch.cc
CXX_TARGETS += src/arch/rm46l8lp/driver/gpio.cc
CXX_TARGETS += src/arch/rm46l8lp/driver/stdout.cc
ASM_TARGETS += src/arch/rm46l8lp/halcogen/dabort.asm
ASM_TARGETS += src/arch/rm46l8lp/halcogen/sys_core.asm
ASM_TARGETS += src/arch/rm46l8lp/halcogen/sys_intvecs.asm
ASM_TARGETS += src/arch/rm46l8lp/halcogen/sys_mpu.asm
ASM_TARGETS += src/arch/rm46l8lp/halcogen/sys_pmu.asm

# special case for TI CC makefile
ifeq (${loop}, 1)
	COMMON_FLAGS += -DCONFIG_loop
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_rm46l8lp_driver_counter = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_rm46l8lp_driver_uptime = y
endif

ifdef CONFIG_arch_rm46l8lp_driver_counter
	CXX_TARGETS += src/arch/rm46l8lp/driver/counter.cc
endif

ifdef CONFIG_arch_rm46l8lp_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/rm46l8lp/driver/uptime.cc
endif

OBJECTS = ${CXX_TARGETS:.cc=.obj} ${C_TARGETS:.c=.obj} ${ASM_TARGETS:.asm=.obj}

%.obj : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} --preproc_with_compile --obj_directory=$(dir $@) ${@:.obj=.cc}

%.obj : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} --preproc_with_compile --obj_directory=$(dir $@) ${@:.obj=.c}

%.obj : %.asm | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} --preproc_with_compile --obj_directory=$(dir $@) ${@:.obj=.asm}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${COMMON_FLAGS} ${CXXFLAGS} \
		-z --heap_size=0x1000 --stack_size=0x1000 \
		-i"${TI_CGT_PATH}/lib" \
		-i"${TI_CGT_PATH}/include" \
		--reread_libs --warn_sections --ecc=off --rom_model -o $@ \
		${OBJECTS} src/arch/rm46l8lp/halcogen/sys_link.cmd \
		-lrtsv7R4_T_le_v3D16_eabi.lib

program: build/system.elf
	${QUIET}${UNIFLASH_PATH}/dslite.sh -c src/arch/rm46l8lp/RM46L852.ccxml -f build/system.elf -u

arch_clean:
	${QUIET}rm -f ${OBJECTS}

monitor:
	${QUIET}screen /dev/${SERIAL_PORT} 19200

arch_help:

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Count Freq: ${counter_freq} Hz"
	@echo "Counter Overflow: 4294967296/4294967295"
	@echo "Monitor: /dev/${SERIAL_PORT} 19200"

attributes: build/system.elf
	${QUIET}script/size.py ${SIZE} intvecs,text,const,cinit,pinit,data data,bss

nfpvalues: build/system.elf
	${QUIET}script/nfpvalues.py ${SIZE} intvecs,text,const,cinit,pinit,data data,bss

.PHONY: arch_clean arch_help arch_info attributes cat monitor program
