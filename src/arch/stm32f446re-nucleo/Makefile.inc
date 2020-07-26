# vim:ft=make

SERIAL_PORT ?= ttyACM0

INCLUDES += -Iext/libopencm3/include

COMMON_FLAGS += --static -nostartfiles -g3 -Os -fno-common
COMMON_FLAGS += -ffunction-sections -fdata-sections
COMMON_FLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F4

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

CXX_TARGETS += src/arch/stm32f446re-nucleo/arch.cc

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 --c_compiler arm-none-eabi-g++ -p . --Xcompiler
endif

ifneq ($(findstring adc,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/adc.cc
endif

CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/gpio.cc
CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/stdout.cc
#CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/uptime.cc

ifneq ($(findstring stdin,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/stdin.cc
endif

ifneq ($(findstring softi2c,${drivers}), )
else ifneq ($(findstring i2c,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/i2c.cc
	COMMON_FLAGS += -DDRIVER_I2C
endif

ifneq ($(findstring spi_a1,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/spi_a1.cc
endif

ifneq ($(findstring spi_b,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/spi_b.cc
endif

ifneq ($(findstring timer,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/timer.cc
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CXX_TARGETS += src/arch/stm32f446re-nucleo/driver/counter.cc
endif


OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o} ${ASM_TARGETS:.S=.o}

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

.S.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -c -o $@ ${@:.o=.S}

.s.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -c -o $@ ${@:.o=.S}

# deliberately no ${MAKE} here -- multipass relies on make -B, but we don't
# want to re-make libopencm3 all the time
ext/libopencm3/lib/libopencm3_stm32f4.a: ext/libopencm3/Makefile
	git submodule update --init
	${MAKE} -C ext/libopencm3

build/system.elf: ${OBJECTS} ext/libopencm3/lib/libopencm3_stm32f4.a
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Lext/libopencm3/lib -Wl,--gc-sections \
		-Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group \
		-T src/arch/stm32f446re-nucleo/stm32f446.ld \
		${OBJECTS} -lopencm3_stm32f4 \
		-o $@

program: build/system.elf
	openocd -c 'source [find interface/stlink-v2-1.cfg]' \
		-c 'transport select hla_swd' -c 'source [find target/stm32f4x.cfg]' \
		-c 'reset_config srst_only' -c '$$_TARGETNAME configure -rtos auto' \
		-c 'reset_config connect_assert_srst' -c init -c targets \
		-c 'reset halt' -c 'flash write_image erase "build/system.elf" 0' \
		'verify_image "build/system.elf" 0' -c 'reset run' -c 'shutdown'

arch_clean:
	${QUIET}rm -f ${OBJECTS}

monitor:
	${QUIET}screen /dev/${SERIAL_PORT} 115200

arch_help:
	@echo "stm32f446re-nucleo specific flags:"
	@echo "    DEBUG_PORT = ${DEBUG_PORT}"
	@echo "    SERIAL_PORT = ${SERIAL_PORT}"
	@echo "    cpu_freq = ${cpu_freq} (desired CPU frequency in Hz)"
	@echo "        supported frequencies: 1 / 4 / 8 / 16 MHz"
	@echo "    MSP430_FLASHER_DIR = /home/derf/var/projects/msp430/MSP430Flasher_1.3.7"
	@echo "        (required for flashing, must contain libmsp430.so and MSP430Flasher)"

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Timer Freq: ${timer_freq} Hz -> $(shell src/arch/stm32f446re-nucleo/model.py f_timer "${cpu_freq}" "${timer_freq}")"
	@echo "I2C   Freq: ${i2c_freq} Hz"
	@echo "Counter Overflow: 65536/255"
	@echo "Monitor: /dev/${SERIAL_PORT} 115200"

.PHONY: arch_clean arch_help arch_info monitor program
