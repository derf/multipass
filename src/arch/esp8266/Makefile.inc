# vim:ft=make

TOOLCHAIN_BASE ?= /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/bin
SDK_BASE ?= /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr
ESPTOOL ?= esptool
PORT ?= /dev/ttyUSB0

CC = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc
CXX = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-g++
AR = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-ar
LD = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc
OBJCOPY = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-objcopy
OBJDUMP = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-objdump

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler ${TOOLCHAIN_BASE}/xtensa-lx106-elf-g++ -p . --Xcompiler
endif

INCLUDES += -I${SDK_BASE}/include
COMMON_FLAGS += -nostdlib -mlongcalls -flto -D__ets__ -DICACHE_FLASH -DMULTIPASS_ARCH_esp8266
CXXFLAGS = -std=c++11 -fno-rtti -fno-threadsafe-statics
LDFLAGS += -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

CXX_TARGETS += src/arch/esp8266/arch.cc src/arch/esp8266/driver/gpio.cc
CXX_TARGETS += src/arch/esp8266/driver/stdout.cc src/arch/esp8266/driver/uptime.cc

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

ifeq (${esp8266_led2}, 1)
	COMMON_FLAGS += -DLED_ON_GPIO16
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	CXX_TARGETS += src/arch/esp8266/driver/stdin.cc
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CXX_TARGETS += src/arch/esp8266/driver/counter.cc
endif

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}
	${QUIET}${OBJCOPY} --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $@

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}
	${QUIET}${OBJCOPY} --rename-section .text=.irom0.text --rename-section .literal=.irom0.literal $@

build/system.ar: ${OBJECTS}
	${AR} cru $@ ${OBJECTS}

build/system.elf: build/system.ar
	${CC} -L${SDK_BASE}/lib -T${SDK_BASE}/lib/eagle.app.v6-derf.ld ${LDFLAGS} \
		-Wl,--gc-sections \
		-Wl,--start-group -lc -lgcc -lhal -lpp -lphy -lnet80211 -llwip -lwpa \
		-lmain $< -Wl,--end-group -o $@

build/0x00000.bin: build/system.elf
	${ESPTOOL} --chip esp8266 elf2image -o build/ $<

build/0x40000.bin: build/0x00000.bin
	# also created by commandline for 0x00000.bin

program: build/0x00000.bin build/0x40000.bin
	${ESPTOOL} write_flash 0x00000 build/0x00000.bin 0x40000 build/0x40000.bin

arch_clean:
	rm -f ${OBJECTS}
	rm -f build/system.ar

monitor:
	screen /dev/ttyUSB0 115200

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
	@echo "    PORT = /dev/ttyUSB0"

arch_info:
	@echo "Counter Overflow: ?/?"
	@echo "Monitor: /dev/ttyUSB0 115200"

.PHONY: arch_clean arch_help arch_info monitor program
