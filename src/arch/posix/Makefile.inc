# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

COMMON_FLAGS += -DMULTIPASS_ARCH_posix

CC = gcc
CXX = g++
OBJDUMP = objdump

ARCH_SHORTNAME = x64

ifdef CONFIG_aspectc
	CXX = ag++ -r build/repo.acp -v 0 -p . --Xcompiler
endif

CXX_TARGETS += src/arch/posix/arch.cc src/arch/posix/driver/gpio.cc
CXX_TARGETS += src/arch/posix/driver/stdout.cc

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_posix_driver_counter = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_posix_driver_uptime = y
endif

ifdef CONFIG_arch_posix_driver_counter
	CXX_TARGETS += src/arch/posix/driver/counter.cc
endif

ifdef CONFIG_arch_posix_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/posix/driver/uptime.cc
endif

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

ifneq (${gpio_trace}, )
	COMMON_FLAGS += -DGPIO_TRACE
endif
ifdef CONFIG_gpio_trace
	COMMON_FLAGS += -DGPIO_TRACE
endif

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

%.o: %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-o $@ ${OBJECTS}

run: build/system.elf
	build/system.elf

monitor: run

program: build/system.elf

arch_clean:
	${QUIET}rm -f ${OBJECTS}

arch_help:
	@echo "posix specific flags:"
	@echo "    gpio_trace = 0 (show GPIO changes on stdout)"

arch_info:
	@echo "Counter Overflow: 18446744073709551616/1"
	@echo "Monitor: run build/system.elf"

attributes: build/system.elf
	${QUIET}script/size.py size text data,bss

.PHONY: arch_clean arch_help arch_info attributes monitor program run
