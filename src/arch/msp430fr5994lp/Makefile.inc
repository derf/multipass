# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

CPU = 430x
MCU = msp430fr5994

DEBUG_PORT ?= ttyACM0
SERIAL_PORT ?= ttyACM1

cpu_freq ?= 16000000
uart_freq ?= 115200

MSP430_FLASHER_DIR ?= /opt/msp430/MSP430Flasher_1.3.20

INCLUDES += -I/opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/include
COMMON_FLAGS += -mcpu=${CPU} -mmcu=${MCU} -DMULTIPASS_ARCH_msp430fr5994lp
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

ifdef CONFIG_arch_msp430fr5994lp_large_mode
	override msp430_large = 1
endif
ifneq (${msp430_large}, )
	COMMON_FLAGS += -mcode-region=either -mlarge -DADDR_20BIT -include int20.h
endif

# LTO seems to be broken.

CC = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-gcc
CXX = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-g++
OBJCOPY = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-objcopy
OBJDUMP = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-objdump
SIZE = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-size
GDB = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-gdb
GDBA = /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/gdb_agent_console

ARCH_SHORTNAME = msp430

CXX_TARGETS += src/arch/msp430fr5994lp/arch.cc
CXX_TARGETS += src/arch/msp430fr5994lp/driver/gpio.cc
CXX_TARGETS += src/arch/msp430fr5994lp/driver/stdout.cc

ifdef CONFIG_aspectc
	ifeq (${msp430_large}, )
		CXX = ag++ -r build/repo.acp -v 0 --c_compiler /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/bin/msp430-elf-g++ -p . --Xcompiler
	else
		CXX = ag++ -r build/repo.acp -v 0 --c_compiler script/g++wrap -p . --Xcompiler
	endif
endif

# Command-line

ifneq ($(findstring adc,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_adc = y
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_stdin = y
endif

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_i2c = y
endif

ifneq ($(findstring spi,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_spi = y
endif

ifneq ($(findstring timer,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_timer = y
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_msp430fr5994lp_driver_counter = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_msp430fr5994lp_driver_uptime = y
endif

ifeq (${with_hfxt}, 1)
	CONFIG_arch_msp430fr5994lp_hfxt_16mhz = y
endif

# Kconfig

ifdef CONFIG_arch_msp430fr5994lp_driver_adc
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/adc.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_stdin
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/stdin.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_i2c
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/i2c.cc
	COMMON_FLAGS += -DDRIVER_I2C
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_spi
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/spi.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_timer
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/timer.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_counter
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/counter.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/uptime.cc
endif

ifdef CONFIG_arch_msp430fr5994lp_hfxt_16mhz
	COMMON_FLAGS += -DWITH_HFXT_16MHZ
endif


ifneq ($(findstring timed_resistive_load,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5994lp/driver/timed_resistive_load.cc
	resistor1_pin ?= p3_0
	resistor2_pin ?= p3_1
	resistor3_pin ?= p3_2
	resistor4_pin ?= p3_3
	COMMON_FLAGS += -DDRIVER_TIMED_RESISTIVE_LOAD
	COMMON_FLAGS += -DTIMED_RESISTIVE_LOAD_PIN1=GPIO::${resistor1_pin}
	COMMON_FLAGS += -DTIMED_RESISTIVE_LOAD_PIN2=GPIO::${resistor2_pin}
	COMMON_FLAGS += -DTIMED_RESISTIVE_LOAD_PIN3=GPIO::${resistor3_pin}
	COMMON_FLAGS += -DTIMED_RESISTIVE_LOAD_PIN4=GPIO::${resistor4_pin}
endif

ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
else
	COMMON_FLAGS += -DF_CPU=16000000UL
endif

ifeq (${cpu_freq}, 32768)
	uart_freq=9600
endif

ifneq (${uart_freq}, )
	COMMON_FLAGS += -DF_UART=${uart_freq}UL
else
	COMMON_FLAGS += -DF_UART=115200UL
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
		-Wl,--library-path=/opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/include/ \
		-Wl,--gc-sections \
		-o $@ ${OBJECTS}
	${QUIET}${SIZE} build/system.elf | tail -n1 | awk '{ print "  ROM: " ($$1+$$2) " (" int(($$1+$$2)*100/49152) "%)     RAM: " ($$2 + $$3) " (" int(($$2+$$3)*100/4096) "%)" }'

build/system.hex: build/system.elf
	${QUIET}${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	${QUIET}LD_LIBRARY_PATH=${MSP430_FLASHER_DIR} \
	${MSP430_FLASHER_DIR}/MSP430Flasher \
	-i ${DEBUG_PORT} \
	-w build/system.hex -v -g -z '[VCC]'

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.hex

cat:
	${QUIET}script/cat.py /dev/${SERIAL_PORT} ${uart_freq} ${cpu_freq} 65536

monitor:
	${QUIET}screen /dev/${SERIAL_PORT} ${uart_freq}

arch_help:
	@echo "msp430fr5994lp specific flags:"
	@echo "    DEBUG_PORT = ${DEBUG_PORT}"
	@echo "    SERIAL_PORT = ${SERIAL_PORT}"
	@echo "    cpu_freq = ${cpu_freq} (desired CPU frequency in Hz)"
	@echo "        supported frequencies: 1 / 4 / 8 / 16 MHz"
	@echo "    MSP430_FLASHER_DIR = /home/derf/var/projects/msp430/MSP430Flasher_1.3.20"
	@echo "        (required for flashing, must contain libmsp430.so and MSP430Flasher)"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz -> $(shell src/arch/msp430fr5994lp/model.py f_timer "${cpu_freq}" "${timer_freq}")"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 65536/65535"
	@echo "sleep_ms Overflow: 250 500"
	@echo "Monitor: /dev/${SERIAL_PORT} ${uart_freq}"

gdb-server:
	${GDBA} /opt/msp430/ti/msp430-gcc-full-linux-5.1.2.0/msp430.dat

gdb:
	${GDB} -ex 'target remote :55000' build/system.elf

ifdef CONFIG_arch_msp430fr5994lp_large_mode
attributes: build/system.elf
	${QUIET}script/size.py ${SIZE} text,lower.text,upper.text,data,lower.data,upper.data data,lower.data,upper.data,bss,lower.bss,upper.bss
else
attributes: build/system.elf
	${QUIET}script/size.py ${SIZE} text,data data,bss
endif

.PHONY: arch_clean arch_help arch_info attributes cat monitor program
