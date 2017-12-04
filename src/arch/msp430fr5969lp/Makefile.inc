# vim:ft=make

CPU = 430x
MCU = msp430fr5969

INCLUDES += -Iinclude/msp430fr5969lp -I/opt/msp430/ti/gcc/include
COMMON_FLAGS += -mcpu=${CPU} -mmcu=${MCU}

CC = /opt/msp430/ti/gcc/bin/msp430-elf-gcc
CXX = /opt/msp430/ti/gcc/bin/msp430-elf-g++
OBJCOPY = /opt/msp430/ti/gcc/bin/msp430-elf-objcopy

TARGETS += src/arch/msp430fr5969lp/arch.cc src/arch/msp430fr5969lp/driver/gpio.cc

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

.PHONY: arch_clean program
