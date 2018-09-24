# vim:ft=make

MCU = attiny88
PORT = /dev/ttyUSB0

COMMON_FLAGS += -mmcu=${MCU} -DF_CPU=8000000UL -DMULTIPASS_ARCH_blinkenrocket
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

TARGETS += src/arch/blinkenrocket/arch.cc
TARGETS += src/arch/blinkenrocket/driver/gpio.cc
TARGETS += src/arch/blinkenrocket/driver/stdout.cc
TARGETS += src/arch/blinkenrocket/driver/uptime.cc

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	TARGETS += src/arch/blinkenrocket/driver/i2c.cc
endif

ifneq (${i2c_freq}, )
	COMMON_FLAGS += -DF_I2C=${i2c_freq}
endif

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	avr-size --format=avr --mcu=${MCU} $@

build/system.hex: build/system.elf
	${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	avrdude -p ${MCU} -c usbasp -U flash:w:build/system.hex

arch_clean:
	rm -f ${OBJECTS} build/system.hex

monitor:
	screen ${PORT} 115200

arch_help:
	@echo "blinkenrocket specific flags:"
	@echo "    PORT = ${PORT}"

.PHONY: arch_clean arch_help monitor program