# vim:ft=make

TOOLCHAIN_BASE = /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/bin
SDK_BASE = /home/derf/var/projects/esp8266/toolchain/xtensa-lx106-elf/xtensa-lx106-elf/sysroot/usr
ESPTOOL = esptool
PORT = /dev/ttyUSB0

CC = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc
CXX = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-g++
AR = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-ar
LD = ${TOOLCHAIN_BASE}/xtensa-lx106-elf-gcc

INCLUDES += -Iinclude/esp8266 -I${SDK_BASE}/include
COMMON_FLAGS += -nostdlib -mlongcalls -D__ets__ -DICACHE_FLASH
CXXFLAGS = -std=c++11
LDFLAGS += -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

TARGETS += src/arch/esp8266/arch.cc src/arch/esp8266/driver/gpio.cc

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.ar: ${OBJECTS}
	${AR} cru $@ ${OBJECTS}

build/system.elf: build/system.ar
	${CC} -L${SDK_BASE}/lib -T${SDK_BASE}/lib/eagle.app.v6.ld ${LDFLAGS} \
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

.PHONY: arch_clean program
