# vim:ft=make
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

SERIAL_PORT ?= ttyACM0

cpu_freq ?= 168000000

INCLUDES += -Iext/libopencm3/include

COMMON_FLAGS += --static -nostartfiles -g3 -Os -fno-common
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

ARCH_SHORTNAME = stm32f4

CXX_TARGETS += src/arch/stm32f446re-nucleo/arch.cc

ifdef CONFIG_aspectc
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler arm-none-eabi-g++ -p . --Xcompiler
endif

ifneq ($(findstring adc,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/adc.cc
endif

CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/gpio.cc
CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/stdout.cc

# Commandline

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_stm32f446re_nucleo_driver_counter = y
endif

#ifneq ($(findstring timer,${arch_drivers}), )
#	CONFIG_arch_stm32f446re_nucleo_driver_timer = y
#endif

ifeq (${timer_s}, 1)
	CONFIG_arch_stm32f446re_nucleo_driver_uptime = y
endif

# Kconfig

ifdef CONFIG_arch_stm32f446re_nucleo_driver_counter
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/counter.cc
endif

ifdef CONFIG_arch_stm32f446re_nucleo_driver_i2c
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/i2c.cc
endif

#ifdef CONFIG_arch_stm32f446re_nucleo_driver_timer
#	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/timer.cc
#endif

ifdef CONFIG_arch_stm32f446re_nucleo_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/uptime.cc
endif


ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
else
	COMMON_FLAGS += -DF_CPU=168000000UL
endif


OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o} ${ASM_TARGETS:.S=.o}

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

%.o : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

%.o : %.S | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -c -o $@ ${@:.o=.S}

%.o : %.s | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -c -o $@ ${@:.o=.S}

build/system.elf: ${OBJECTS} ext/libopencm3/lib/libopencm3_stm32f4.a
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Lext/libopencm3/lib -Wl,--gc-sections \
		-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
		-T src/arch/stm32f446re-nucleo/stm32f446.ld \
		${OBJECTS} -lopencm3_stm32f4 \
		-o $@
	${QUIET}${SIZE} build/system.elf | tail -n1 | awk '{ print "  ROM: " ($$1+$$2) " (" int(($$1+$$2)*100/524288) "%)     RAM: " ($$2 + $$3) " (" int(($$2+$$3)*100/131072) "%)" }'

ext/libopencm3/Makefile:
	git submodule update --init

ext/libopencm3/lib/libopencm3_stm32f4.a: ext/libopencm3/Makefile
	${MAKE} -C ext/libopencm3

program: build/system.elf
	openocd -c 'source [find interface/stlink.cfg]' \
		-c 'transport select hla_swd' -c 'source [find target/stm32f4x.cfg]' \
		-c 'reset_config srst_only' -c '$$_TARGETNAME configure -rtos auto' \
		-c 'reset_config connect_assert_srst' -c init -c targets \
		-c 'reset halt' -c 'flash write_image erase "build/system.elf" 0' \
		-c 'verify_image "build/system.elf" 0' -c 'reset run' -c 'shutdown'

arch_clean:
	${QUIET}rm -f ${OBJECTS}

cat:
	${QUIET}script/cat.py /dev/${SERIAL_PORT} 115200 ${cpu_freq} 65536

monitor:
	${QUIET}screen /dev/${SERIAL_PORT} 115200

arch_help:
	@echo "stm32f446re-nucleo specific flags:"
	@echo "    cpu_freq = ${cpu_freq} (desired CPU frequency in Hz)"
	@echo "        supported frequencies: 84 / 168 / 180 MHz"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz -> $(shell src/arch/stm32f446re-nucleo/model.py f_timer "${cpu_freq}" "${timer_freq}")"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 4294967296/255"
	@echo "Monitor: /dev/${SERIAL_PORT} 115200"

attributes: build/system.elf
	${QUIET}script/size.py ${SIZE} text,data data,bss

nfpvalues: build/system.elf
	${QUIET}script/nfpvalues.py ${SIZE} text,data data,bss

.PHONY: arch_clean arch_help arch_info attributes cat monitor program
