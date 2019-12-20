# vim:ft=make

MCU = atmega168
SERIAL_PORT ?= /dev/ttyUSB0
BAUD = 19200

cpu_freq ?= 16000000

COMMON_FLAGS += -Werror=overflow
COMMON_FLAGS += -mmcu=${MCU} -DMULTIPASS_ARCH_arduino_nano
COMMON_FLAGS += -DF_CPU=${cpu_freq}UL
COMMON_FLAGS += -DMULTIPASS_ARCH_HAS_I2C

ifeq (${stack_usage}, )
	COMMON_FLAGS += -flto
endif

CC = avr-gcc
CXX = avr-g++
NM = avr-nm
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

ARCH_SHORTNAME = avr

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler avr-g++ -p . --Xcompiler
endif

CXX_TARGETS += src/arch/arduino-nano/arch.cc
CXX_TARGETS += src/arch/arduino-nano/driver/gpio.cc
CXX_TARGETS += src/arch/arduino-nano/driver/stdout.cc
CXX_TARGETS += src/arch/arduino-nano/driver/uptime.cc

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CXX_TARGETS += src/arch/arduino-nano/driver/i2c.cc
endif

ifneq ($(findstring spi,${arch_drivers}), )
	CXX_TARGETS += src/arch/arduino-nano/driver/spi.cc
endif

ifneq ($(findstring stdin,${arch_drivers}), )
	CXX_TARGETS += src/arch/arduino-nano/driver/stdin.cc
endif

ifneq ($(findstring timer,${arch_drivers}), )
	CXX_TARGETS += src/arch/arduino-nano/driver/timer.cc
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CXX_TARGETS += src/arch/arduino-nano/driver/counter.cc
endif

ifeq (${cpu_freq}, 16000000)
	uart_baud = 57600
else ifeq (${cpu_freq}, 8000000)
	uart_baud = 38400
else ifeq (${cpu_freq}, 4000000)
	uart_baud = 38400
else ifeq (${cpu_freq}, 2000000)
	uart_baud = 19200
else ifeq (${cpu_freq}, 1000000)
	uart_baud = 9600
else ifeq (${cpu_freq}, 500000)
	uart_baud = 4800
else ifeq (${cpu_freq}, 250000)
	uart_baud = 2400
else ifeq (${cpu_freq}, 125000)
	uart_baud = 1200
else ifeq (${cpu_freq}, 62500)
	uart_baud = 300
else
	uart_baud = 9600
endif

COMMON_FLAGS += -DBAUD=${uart_baud}UL

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}${CXX} ${COMMON_FLAGS} ${CXXFLAGS} -Wl,--gc-sections -o $@ ${OBJECTS}
	${QUIET}avr-size --format=avr --mcu=${MCU} $@
	${QUIET}test $$(avr-size --format=avr --mcu=${MCU} build/system.elf | fgrep Program | grep -o '[0-9.]*%' | cut -d . -f 1) -lt 100

build/system.hex: build/system.elf
	${QUIET}${OBJCOPY} -O ihex ${@:.hex=.elf} $@

program: build/system.hex
	${QUIET}avrdude -p ${MCU} -c arduino -P ${SERIAL_PORT} -b ${BAUD} -U flash:w:build/system.hex

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.hex

monitor:
	${QUIET}screen ${SERIAL_PORT} ${uart_baud}

arch_help:
	@echo "arduino-nano specific flags:"
	@echo " SERIAL_PORT = ${SERIAL_PORT}"
	@echo "        BAUD = ${BAUD} (only used for programming)"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 65536/255"
	@echo "Monitor: ${SERIAL_PORT} ${uart_baud}"

.PHONY: arch_clean arch_help arch_info monitor program
