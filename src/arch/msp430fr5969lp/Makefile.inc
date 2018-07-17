# vim:ft=make

CPU = 430x
MCU = msp430fr5969

INCLUDES += -I/opt/msp430/ti/gcc/include
COMMON_FLAGS += -mcpu=${CPU} -mmcu=${MCU} -DMULTIPASS_ARCH_msp430fr5969lp
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

CC = /opt/msp430/ti/gcc/bin/msp430-elf-gcc
CXX = /opt/msp430/ti/gcc/bin/msp430-elf-g++
OBJCOPY = /opt/msp430/ti/gcc/bin/msp430-elf-objcopy

TARGETS += src/arch/msp430fr5969lp/arch.cc

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler /opt/msp430/ti/gcc/bin/msp430-elf-g++ -p . --Xcompiler
endif

ifneq ($(findstring adc,${arch_drivers}), )
	TARGETS += src/arch/msp430fr5969lp/driver/adc.cc
endif

TARGETS += src/arch/msp430fr5969lp/driver/gpio.cc
TARGETS += src/arch/msp430fr5969lp/driver/stdout.cc
TARGETS += src/arch/msp430fr5969lp/driver/uptime.cc

ifneq ($(findstring stdin,${arch_drivers}), )
	TARGETS += src/arch/msp430fr5969lp/driver/stdin.cc
endif

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	TARGETS += src/arch/msp430fr5969lp/driver/i2c.cc
endif

ifneq ($(findstring spi_a1,${arch_drivers}), )
	TARGETS += src/arch/msp430fr5969lp/driver/spi_a1.cc
endif

ifneq ($(findstring spi_b,${arch_drivers}), )
	TARGETS += src/arch/msp430fr5969lp/driver/spi_b.cc
endif

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Wl,--library-path=/opt/msp430/ti/gcc/include/ \
		-flto -o $@ ${OBJECTS}

build/system.hex: build/system.elf
	${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	LD_LIBRARY_PATH=/home/derf/var/projects/msp430/MSP430Flasher_1.3.7 \
	/home/derf/var/projects/msp430/MSP430Flasher_1.3.7/MSP430Flasher \
	-w build/system.hex -v -g -z '[VCC]'

arch_clean:
	rm -f ${OBJECTS}
	rm -f build/system.hex

monitor:
	screen /dev/ttyACM1 115200

arch_help:
	@echo "msp430fR5969lp specific flags:"
	@echo "    - none -"

.PHONY: arch_clean arch_help monitor program
