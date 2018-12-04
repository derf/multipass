# vim:ft=make

CPU = 430x
MCU = msp430fr5969

cpu_freq ?= 16000000

INCLUDES += -I/opt/msp430/ti/gcc/include
COMMON_FLAGS += -mcpu=${CPU} -mmcu=${MCU} -DMULTIPASS_ARCH_msp430fr5969lp
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

# LTO seems to be broken.

CC = /opt/msp430/ti/gcc/bin/msp430-elf-gcc
CXX = /opt/msp430/ti/gcc/bin/msp430-elf-g++
OBJCOPY = /opt/msp430/ti/gcc/bin/msp430-elf-objcopy

CXX_TARGETS += src/arch/msp430fr5969lp/arch.cc

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler /opt/msp430/ti/gcc/bin/msp430-elf-g++ -p . --Xcompiler
endif

ifneq ($(findstring adc,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/adc.cc
endif

CXX_TARGETS += src/arch/msp430fr5969lp/driver/gpio.cc
CXX_TARGETS += src/arch/msp430fr5969lp/driver/stdout.cc
CXX_TARGETS += src/arch/msp430fr5969lp/driver/uptime.cc

ifneq ($(findstring stdin,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/stdin.cc
endif

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/i2c.cc
	COMMON_FLAGS += -DDRIVER_I2C
endif

ifneq ($(findstring spi_a1,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/spi_a1.cc
endif

ifneq ($(findstring spi_b,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/spi_b.cc
endif

ifneq ($(findstring timer,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/timer.cc
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CXX_TARGETS += src/arch/msp430fr5969lp/driver/counter.cc
endif

ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
else
	COMMON_FLAGS += -DF_CPU=16000000UL
endif


OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Wl,--library-path=/opt/msp430/ti/gcc/include/ \
		-Wl,--gc-sections \
		-o $@ ${OBJECTS}

build/system.hex: build/system.elf
	${QUIET}${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	${QUIET}LD_LIBRARY_PATH=/home/derf/var/projects/msp430/MSP430Flasher_1.3.7 \
	/home/derf/var/projects/msp430/MSP430Flasher_1.3.7/MSP430Flasher \
	-w build/system.hex -v -g -z '[VCC]'

arch_clean:
	${QUIET}rm -f ${OBJECTS}
	${QUIET}rm -f build/system.hex

monitor:
	${QUIET}screen /dev/ttyACM1 115200

arch_help:
	@echo "msp430fR5969lp specific flags:"
	@echo "    - none -"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz -> $(shell src/arch/msp430fr5969lp/model.py f_timer "${cpu_freq}" "${timer_freq}")"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Monitor: /dev/ttyACM1 115200"

.PHONY: arch_clean arch_help arch_info monitor program
