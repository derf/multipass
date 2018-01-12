# vim:ft=make

MCU = atmega328p
PROGRAMMER ?= usbasp
PORT = /dev/ttyUSB0
BAUD = 57600

INCLUDES += -Iinclude/arduino-nano
COMMON_FLAGS += -mmcu=${MCU} -DF_CPU=16000000UL

CC = avr-gcc
CXX = avr-g++
NM = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler avr-g++ -p . --Xcompiler
endif

TARGETS += src/arch/arduino-nano/arch.cc
TARGETS += src/arch/arduino-nano/driver/gpio.cc
TARGETS += src/arch/arduino-nano/driver/stdout.cc
TARGETS += src/arch/arduino-nano/driver/uptime.cc

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	avr-size --format=avr --mcu=${MCU} $@

build/system.hex: build/system.elf
	${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	avrdude -p ${MCU} -c arduino -P ${PORT} -b ${BAUD} -U flash:w:build/system.hex

arch_clean:
	rm -f ${OBJECTS} build/system.hex

monitor:
	screen ${PORT} 115200

arch_help:
	@echo "arduino-nano specific flags:"
	@echo "    PORT = ${PORT}"
	@echo "    BAUD = ${BAUD} (only used for programming)"

.PHONY: arch_clean arch_help monitor program