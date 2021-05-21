# vim:ft=make
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

TOOLCHAIN_BASE ?= /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/bin
SDK_BASE ?= /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr
ESPTOOL ?= esptool
SERIAL_PORT ?= /dev/ttyUSB0

CC = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc
CXX = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-g++
AR = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-ar
LD = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc
OBJCOPY = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-objcopy
OBJDUMP = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-objdump
SIZE = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-size

ifdef CONFIG_aspectc
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler ${TOOLCHAIN_BASE}/xtensa-lx106-elf-g++ -p . --Xcompiler
endif

INCLUDES += -I${SDK_BASE}/include
COMMON_FLAGS += -nostdlib -mlongcalls -flto -D__ets__ -DICACHE_FLASH -DMULTIPASS_ARCH_esp8266
CXXFLAGS = -std=c++11 -fno-rtti -fno-threadsafe-statics
LDFLAGS += -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

CXX_TARGETS += src/arch/esp8266/arch.cc src/arch/esp8266/driver/gpio.cc
CXX_TARGETS += src/arch/esp8266/driver/stdout.cc

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

ifeq (${esp8266_led2}, 1)
	COMMON_FLAGS += -DLED_ON_GPIO16
endif

# Commandline

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_esp8266_driver_counter = y
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	CONFIG_arch_esp8266_driver_stdin = y
endif

ifeq (${timer_s}, 1)
	CONFIG_arch_esp8266_driver_uptime = y
endif

# Kconfig

ifdef CONFIG_arch_esp8266_driver_counter
	CXX_TARGETS += src/arch/esp8266/driver/counter.cc
endif

ifdef CONFIG_arch_esp8266_driver_stdin
	CXX_TARGETS += src/arch/esp8266/driver/stdin.cc
endif

ifdef CONFIG_arch_esp8266_driver_uptime
	COMMON_FLAGS += -DTIMER_S
	CXX_TARGETS += src/arch/esp8266/driver/uptime.cc
endif

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}
	${QUIET}${OBJCOPY} --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $@

%.o : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}
	${QUIET}${OBJCOPY} --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $@

build/system.elf: build/system.ar
	${QUIET}${CC} -L${SDK_BASE}/lib -T${SDK_BASE}/lib/eagle.app.v6-derf.ld ${LDFLAGS} \
		-Wl,--gc-sections \
		-Wl,--start-group -lc -lgcc -lhal -lpp -lphy -lnet80211 -llwip -lwpa \
		-lmain $< -Wl,--end-group -o $@
	${QUIET}${SIZE} build/system.elf | tail -n1 | awk '{ print "  ROM: " ($$1+$$2) " (" int(($$1+$$2)*100/4194304) "%)     RAM: " ($$2 + $$3) " (" int(($$2+$$3)*100/81920) "%)" }'

build/system.ar: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${AR} cru $@ ${OBJECTS}

build/0x00000.bin: build/system.elf
	${ESPTOOL} --chip esp8266 elf2image -o build/ $<

build/0x40000.bin: build/0x00000.bin
	# also created by commandline for 0x00000.bin

program: build/0x00000.bin build/0x40000.bin
	${ESPTOOL} -p ${SERIAL_PORT} write_flash 0x00000 build/0x00000.bin 0x40000 build/0x40000.bin #-fm dout

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.ar

monitor:
	screen ${SERIAL_PORT} 115200

cat:
	${QUIET}script/cat.py ${SERIAL_PORT} 115200 80000000 0

arch_help:
	@echo "esp8266 coding advice:"
	@echo "    The ESP8266 SDK does not give full hardware control."
	@echo "    Applications must release control every hundred or so milliseconds."
	@echo "    It is strongly recommended to only use loop=1"
	@echo
	@echo "esp8266 specific flags:"
	@echo "    esp8266_led2 = 0 (enable if you are using a NodeMCU dev board with two LEDs)"
	@echo "    TOOLCHAIN_BASE = /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/bin"
	@echo "    SDK_BASE = /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr"
	@echo "    ESPTOOL = esptool"
	@echo "    SERIAL_PORT = /dev/ttyUSB0"

arch_info:
	@echo "Counter Overflow: 4294967296/0"
	@echo "Monitor: ${SERIAL_PORT} 115200"

attributes: build/system.elf
	${QUIET}script/size.py ${SIZE} text,irom0.text data,bss

.PHONY: arch_clean arch_help arch_info attributes cat monitor program
