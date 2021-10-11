# vim:ft=make
#
# Copyright 2021 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

MCU = atmega2560
SERIAL_PORT ?= /dev/ttyUSB0
BAUD = 115200

ifdef CONFIG_arch_atmega2560_cpufreq
	cpu_freq = ${CONFIG_arch_atmega2560_cpufreq}
endif

cpu_freq ?= 16000000

COMMON_FLAGS += -Werror=overflow
COMMON_FLAGS += -mmcu=${MCU} -DMULTIPASS_ARCH_atmega2560
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

ifdef CONFIG_aspectc
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler avr-g++ -p . --Xcompiler
endif

CXX_TARGETS += src/arch/atmega2560/arch.cc
CXX_TARGETS += src/arch/atmega2560/driver/gpio.cc
CXX_TARGETS += src/arch/atmega2560/driver/stdout.cc

# Command-line driver selection

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_i2c = y
endif

ifneq ($(findstring adc,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_adc = y
endif

ifneq ($(findstring spi,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_spi = y
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_stdin = y
endif

ifneq ($(findstring timer,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_timer = y
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_counter = y
endif

ifneq ($(findstring neopixel,${arch_drivers}), )
	CONFIG_arch_atmega2560_driver_neopixel = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_atmega2560_driver_uptime = y
endif

# Kconfig driver selection

ifdef CONFIG_arch_atmega2560_driver_i2c
	CXX_TARGETS += src/arch/atmega2560/driver/i2c.cc
endif

ifdef CONFIG_arch_atmega2560_driver_adc
	CXX_TARGETS += src/arch/atmega2560/driver/adc.cc
endif

ifdef CONFIG_arch_atmega2560_driver_spi
	CXX_TARGETS += src/arch/atmega2560/driver/spi.cc
endif

ifdef CONFIG_arch_atmega2560_driver_stdin
	CXX_TARGETS += src/arch/atmega2560/driver/stdin.cc
endif

ifdef CONFIG_arch_atmega2560_driver_timer
	CXX_TARGETS += src/arch/atmega2560/driver/timer.cc
endif

ifdef CONFIG_arch_atmega2560_driver_counter
	CXX_TARGETS += src/arch/atmega2560/driver/counter.cc
endif

ifdef CONFIG_arch_atmega2560_driver_neopixel
	CXX_TARGETS += src/arch/atmega2560/driver/neopixel.cc
endif

ifdef CONFIG_arch_atmega2560_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/atmega2560/driver/uptime.cc
endif

ifeq (${cpu_freq}, 16000000)
	uart_baud = 57600
else ifeq (${cpu_freq}, 8000000)
	uart_baud = 38400
else ifeq (${cpu_freq}, 4000000)
	uart_baud = 38400
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
	${QUIET}avrdude -p ${MCU} -c wiring -P ${SERIAL_PORT} -b ${BAUD} -D -U flash:w:build/system.hex

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
	@echo "atmega2560 specific flags:"
	@echo " SERIAL_PORT = ${SERIAL_PORT}"
	@echo "        BAUD = ${BAUD} (only used for programming)"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 65536/255"
	@echo "Monitor: ${SERIAL_PORT} ${uart_baud}"

attributes: build/system.elf
	${QUIET}script/size.py avr-size text,data data,bss

nfpvalues: build/system.elf
	${QUIET}script/nfpvalues.py avr-size text,data data,bss

.PHONY: arch_clean arch_help arch_info attributes cat monitor program size
