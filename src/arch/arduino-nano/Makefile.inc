# vim:ft=make

MCU = atmega328p
PORT = /dev/ttyUSB0
BAUD = 57600

cpu_freq ?= 16000000

COMMON_FLAGS += -Werror=overflow
COMMON_FLAGS += -mmcu=${MCU} -DMULTIPASS_ARCH_arduino_nano
COMMON_FLAGS += -flto
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

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

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	TARGETS += src/arch/arduino-nano/driver/i2c.cc
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	TARGETS += src/arch/arduino-nano/driver/stdin.cc
endif

ifneq ($(findstring timer,${arch_drivers}), )
	TARGETS += src/arch/arduino-nano/driver/timer.cc
endif

ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
else
	COMMON_FLAGS += -DF_CPU=16000000UL
endif

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${QUIET}${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	${QUIET}avr-size --format=avr --mcu=${MCU} $@

build/system.hex: build/system.elf
	${QUIET}${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	${QUIET}avrdude -p ${MCU} -c arduino -P ${PORT} -b ${BAUD} -U flash:w:build/system.hex

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.hex

monitor:
	${QUIET}screen ${PORT} 19200

arch_help:
	@echo "arduino-nano specific flags:"
	@echo "    PORT = ${PORT}"
	@echo "    BAUD = ${BAUD} (only used for programming)"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz"
	@echo "I2C   Freq: ${i2c_freq} Hz"

.PHONY: arch_clean arch_help arch_info monitor program
