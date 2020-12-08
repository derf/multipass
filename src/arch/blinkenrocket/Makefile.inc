# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

MCU = attiny88
SERIAL_PORT ?= /dev/ttyUSB0

COMMON_FLAGS += -mmcu=${MCU} -DF_CPU=8000000UL -DMULTIPASS_ARCH_blinkenrocket
COMMON_FLAGS += -flto
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

CC = avr-gcc
CXX = avr-g++
NM = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

ARCH_SHORTNAME = avr

ifdef CONFIG_aspectc
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler avr-g++ -p . --Xcompiler
endif

CXX_TARGETS += src/arch/blinkenrocket/arch.cc
CXX_TARGETS += src/arch/blinkenrocket/driver/gpio.cc
CXX_TARGETS += src/arch/blinkenrocket/driver/stdout.cc

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CONFIG_arch_blinkenrocket_driver_i2c = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_blinkenrocket_driver_uptime = y
endif

ifdef CONFIG_arch_blinkenrocket_driver_i2c
	CXX_TARGETS += src/arch/blinkenrocket/driver/i2c.cc
endif

ifdef CONFIG_arch_blinkenrocket_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/blinkenrocket/driver/uptime.cc
endif

ifeq (${kout_nop}, 1)
	COMMON_FLAGS += -DKOUT_NOP
endif
ifdef CONFIG_arch_blinkenrocket_kout_nop
	COMMON_FLAGS += -DKOUT_NOP
endif

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	avr-size --format=avr --mcu=${MCU} $@

build/system.hex: build/system.elf
	${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	avrdude -p ${MCU} -c usbasp -U flash:w:build/system.hex

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.hex

monitor:
	screen ${SERIAL_PORT} 115200

arch_help:
	@echo "blinkenrocket specific flags:"
	@echo "    SERIAL_PORT = ${SERIAL_PORT}"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: ?/?"
	@echo "Monitor: ${SERIAL_PORT} 115200"

attributes: build/system.elf
	${QUIET}script/size.py avr-size text,data data,bss

.PHONY: arch_clean arch_help arch_info attributes monitor program
