# vim:ft=make
#
# Copyright 2022 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

CPU = tc39xx

COMMON_FLAGS += -mcpu=${CPU} -DMULTIPASS_ARCH_tc397

ARCH_SHORTNAME = tc397

LICENSE = -mlicense-dir=${HOME}/var/source/aurix-infineon-hightec-tricore
CC =      wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-gcc.exe ${LICENSE}
CXX =     wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-g++.exe ${LICENSE}
OBJCOPY = wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-objcopy.exe ${LICENSE}
OBJDUMP = wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-objdump.exe ${LICENSE}
SIZE =    wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-size.exe

CXX_TARGETS += src/arch/infineon-tc397-tft-kit/arch.cc
CXX_TARGETS += src/arch/infineon-tc397-tft-kit/driver/gpio.cc
CXX_TARGETS += src/arch/infineon-tc397-tft-kit/driver/stdout.cc

ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
else
	COMMON_FLAGS += -DF_CPU=300000000UL
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_infineon_tc397_tft_kit_driver_counter = y
endif

ifdef CONFIG_arch_infineon_tc397_tft_kit_driver_counter
	CXX_TARGETS += src/arch/infineon-tc397-tft-kit/driver/counter.cc
endif

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o} ${ASM_TARGETS:.S=.o}

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

%.o : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

%.o : %.S | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -Wa,-gstabs,-ggdb -x assembler-with-cpp -c -o $@ ${@:.o=.S}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Wl,--gc-sections \
		-o $@ ${OBJECTS}

program: build/system.elf
	@echo "Not Implemented"
	${QUIET}false

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.elf

monitor:
	@echo "Not Implemented"
	${QUIET}false

arch_help:
	@true

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"

attributes: build/system.elf
	${QUIET}script/size.py "${SIZE}" text,rodata bss

nfpvalues: build/system.elf
	${QUIET}script/nfpvalues.py "${SIZE}" text,rodata bss

.PHONY: arch_clean arch_help arch_info attributes monitor program
