# vim:ft=make
#
# Copyright 2021 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

MCU = atmega32u4
SERIAL_PORT ?= /dev/ttyUSB0

ifdef CONFIG_arch_lora32u4ii_cpufreq
	cpu_freq = ${CONFIG_arch_lora32u4ii_cpufreq}
endif

cpu_freq ?= 8000000

COMMON_FLAGS += -Werror=overflow
COMMON_FLAGS += -mmcu=${MCU} -DMULTIPASS_ARCH_lora32u4ii
COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

ifeq (${stack_usage}, )
	COMMON_FLAGS += -flto
endif

CC = avr-gcc
CXX = avr-g++
NM = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

ARCH_SHORTNAME = avr

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler avr-g++ -p . --Xcompiler
endif

CXX_TARGETS += src/arch/lora32u4ii/arch.cc
CXX_TARGETS += src/arch/lora32u4ii/driver/gpio.cc
CXX_TARGETS += src/arch/lora32u4ii/driver/stdout.cc

# Command-line driver selection

ifeq (${timer_s}, 1)
	CONFIG_arch_lora32u4ii_driver_uptime = y
endif

ifneq ($(findstring adc,${arch_drivers}), )
	CONFIG_arch_lora32u4ii_driver_adc = y
endif

# Kconfig driver selection

ifdef CONFIG_arch_lora32u4ii_driver_adc
	CXX_TARGETS += src/arch/lora32u4ii/driver/adc.cc
endif

ifdef CONFIG_arch_lora32u4ii_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/lora32u4ii/driver/uptime.cc
endif

ifeq (${cpu_freq}, 8000000)
	uart_baud = 19200
else ifeq (${cpu_freq}, 4000000)
	uart_baud = 19200
else ifeq (${cpu_freq}, 2000000)
	uart_baud = 19200
else ifeq (${cpu_freq}, 1000000)
	uart_baud = 9600
else ifeq (${cpu_freq}, 500000)
	uart_baud = 4800
else ifeq (${cpu_freq}, 250000)
	uart_baud = 2400
else ifeq (${cpu_freq}, 125000)
	uart_baud = 1200
else ifeq (${cpu_freq}, 62500)
	uart_baud = 300
else
	uart_baud = 9600
endif

COMMON_FLAGS += -DBAUD=${uart_baud}UL

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

%.o : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	${QUIET}avr-size --format=avr --mcu=${MCU} $@
	${QUIET}test $$(avr-size --format=avr --mcu=${MCU} build/system.elf | fgrep Program | grep -o '[0-9.]*%' | cut -d . -f 1) -lt 100

build/system.hex: build/system.elf
	${QUIET}${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	${QUIET}avrdude -p ${MCU} -c usbasp -U flash:w:build/system.hex

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.hex

cat:
	${QUIET}script/cat.py ${SERIAL_PORT} ${uart_baud} ${cpu_freq} 65536

monitor:
	${QUIET}screen ${SERIAL_PORT} ${uart_baud}

size: build/system.elf
	${QUIET}avr-size --format=avr --mcu=${MCU} build/system.elf | fgrep Program | perl -nE 'if (m{(\d+) bytes \(([0-9.]+%)}) { print("$$1;$$2;") }'
	${QUIET}avr-size --format=avr --mcu=${MCU} build/system.elf | fgrep Data | perl -nE 'if (m{(\d+) bytes \(([0-9.]+%)}) { print("$$1;$$2;") }'
	${QUIET}echo

arch_help:
	@echo "lora32u4ii specific flags:"
	@echo " SERIAL_PORT = ${SERIAL_PORT}"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 65536/255"
	@echo "Monitor: ${SERIAL_PORT} ${uart_baud}"

.PHONY: arch_clean arch_help arch_info monitor program
