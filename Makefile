default: build/system.elf

INCLUDES = -Iinclude -Iinclude/arch/${arch}
COMMON_FLAGS = -g -Os -Wall -Wextra
CFLAGS = -std=c99
CXXFLAGS = -std=c++14 -fno-rtti -fno-threadsafe-statics -fno-exceptions

ASM_TARGETS =
C_TARGETS =
CXX_TARGETS = src/app/${app}/main.cc src/os/object/cpp_helpers.cc src/os/object/outputstream.cc

QUIET = @

ifneq (${verbose}, )
	QUIET =
endif

ifneq (${stack_usage}, )
	COMMON_FLAGS += -fstack-usage
endif

ifneq (${app}, )
	include src/app/${app}/Makefile.inc
endif

ifneq ($(findstring dummy,${drivers}), )
	CXX_TARGETS += src/driver/dummy.cc
	COMMON_FLAGS += -DDRIVER_DUMMY
endif

ifneq ($(findstring lm75,${drivers}), )
	CXX_TARGETS += src/driver/lm75.cc
	COMMON_FLAGS += -DDRIVER_LM75
endif

ifneq ($(findstring am2320,${drivers}), )
	CXX_TARGETS += src/driver/am2320.cc
	COMMON_FLAGS += -DDRIVER_AM2320
endif

ifneq ($(findstring bme680,${drivers}), )
	CXX_TARGETS += src/driver/bme680.cc src/driver/bme680_util.cc
	COMMON_FLAGS += -DDRIVER_BME680
endif

ifneq ($(findstring ccs811,${drivers}), )
	CXX_TARGETS += src/driver/ccs811.cc
	COMMON_FLAGS += -DDRIVER_CCS811
endif

ifneq ($(findstring eeprom24lc64,${drivers}), )
	CXX_TARGETS += src/driver/eeprom24lc64.cc
	COMMON_FLAGS += -DDRIVER_EEPROM24LC64
endif

ifneq ($(findstring max44006,${drivers}), )
	CXX_TARGETS += src/driver/max44006.cc
	COMMON_FLAGS += -DDRIVER_MAX44006
endif

ifneq ($(findstring max44009,${drivers}), )
	CXX_TARGETS += src/driver/max44009.cc
	COMMON_FLAGS += -DDRIVER_MAX44009
endif

ifneq ($(findstring hdc1080,${drivers}), )
	CXX_TARGETS += src/driver/hdc1080.cc
	COMMON_FLAGS += -DDRIVER_HDC1080
endif

ifneq ($(findstring mpu9250,${drivers}), )
	CXX_TARGETS += src/driver/mpu9250.cc
	COMMON_FLAGS += -DDRIVER_MPU9250
endif

ifneq ($(findstring mmsimple,${drivers}), )
	CXX_TARGETS += src/driver/mmsimple.cc
	COMMON_FLAGS += -DDRIVER_MMSIMPLE
endif

ifneq ($(findstring nrf24l01,${drivers}), )
	CXX_TARGETS += src/driver/nrf24l01.cc
	ifeq (${arch}, msp430fr5994lp)
	nrf24l01_cs_pin ?= p1_3
	nrf24l01_en_pin ?= p6_2
	nrf24l01_irq_pin ?= p8_3
	endif
	ifeq (${arch}, msp430fr5969lp)
		nrf24l01_en_pin ?= p4_3
		nrf24l01_cs_pin ?= p2_4
		nrf24l01_irq_pin ?= p1_5
	endif
	COMMON_FLAGS += -DDRIVER_NRF24L01
	COMMON_FLAGS += -DNRF24L01_EN_PIN=GPIO::${nrf24l01_en_pin}
	COMMON_FLAGS += -DNRF24L01_CS_PIN=GPIO::${nrf24l01_cs_pin}
	COMMON_FLAGS += -DNRF24L01_IRQ_PIN=GPIO::${nrf24l01_irq_pin}
endif

ifneq ($(findstring sharp96,${drivers}), )
	CXX_TARGETS += src/driver/sharp96.cc
	sharp96_power_pin ?= p4_2
	sharp96_en_pin ?= p4_3
	sharp96_cs_pin ?= p2_4
	sharp96_power_pin ?= p1_2
	sharp96_en_pin ?= p6_2
	sharp96_cs_pin ?= p1_3
	COMMON_FLAGS += -DDRIVER_SHARP6
	COMMON_FLAGS += -DSHARP96_POWER_PIN=GPIO::${sharp96_power_pin}
	COMMON_FLAGS += -DSHARP96_EN_PIN=GPIO::${sharp96_en_pin}
	COMMON_FLAGS += -DSHARP96_CS_PIN=GPIO::${sharp96_cs_pin}
endif

ifneq ($(findstring softi2c,${drivers}), )
	CXX_TARGETS += src/driver/soft_i2c.cc
	COMMON_FLAGS += -DDRIVER_SOFTI2C
endif

ifneq (${i2c_freq}, )
	COMMON_FLAGS += -DF_I2C=${i2c_freq}
endif

ifneq (${timer_freq}, )
	COMMON_FLAGS += -DF_TIMER=${timer_freq}
endif

ifeq (${softi2c_pullup}, internal)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_INTERNAL
endif

ifeq (${softi2c_pullup}, external)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_EXTERNAL
endif

ifeq (${softi2c_pullup}, gpio)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_FIXED_GPIO
endif

ifeq (${i2c_pullup}, gpio)
	COMMON_FLAGS += -DI2C_PULLUP_FIXED_GPIO
endif

ifeq (${softi2c_timer}, 1)
	arch_drivers += ,timer
	COMMON_FLAGS += -DSOFTI2C_TIMER
endif

ifeq (${kout_nop}, 1)
	COMMON_FLAGS += -DKOUT_NOP
endif

ifeq (${timer_cycles}, 1)
	COMMON_FLAGS += -DTIMER_CYCLES
endif

ifeq (${timer_us}, 1)
	COMMON_FLAGS += -DTIMER_US
endif

ifeq (${timer_s}, 1)
	COMMON_FLAGS += -DTIMER_S
endif

ifeq (${loop}, 1)
	COMMON_FLAGS += -DWITH_LOOP
endif

ifeq (${wakeup}, 1)
	COMMON_FLAGS += -DWITH_WAKEUP
endif

ifeq (${ostream}, 1)
	COMMON_FLAGS += -DWITH_OSTREAM
endif

ifeq (${trace_malloc}, 1)
	CXX_TARGETS += src/lib/mpmalloc.cc
	COMMON_FLAGS += -DMULTIPASS_TRACE_MALLOC
endif

ifneq (${arch}, )
	include src/arch/${arch}/Makefile.inc
endif

stack: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

stackm: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl --machine-readable ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

clean: arch_clean
	rm -f build/system.elf

help: arch_help
	@echo
	@echo "common flags:"
	@echo "    timer_cycles timer_us timer_s -- enable specific timers. May be mutually exclusive"
	@echo "    loop -- enable loop() function"
	@echo "    i2c_freq -- I2C Frequency in Hz"
	@echo "    timer_freq -- Timer frequency in Hz"
	@echo "    softi2c_pullup -- When using softi2c driver: configure SDA/SCL pull-ups"
	@echo "        internal: Use internal pull-ups. Pull-ups are disabled when transmitting a LOW signal"
	@echo "        external: Use external pull-ups connected to two separate CPU pins. Pull-Ups are disabled when transmitting a LOW signal"
	@echo "        gpio: Use external pull-ups connected to two separate CPU pins. Pull-Ups are always enabled"
	@echo "    softi2c_timer -- Use hardware timer instead of busy waiting for I2C timing"
	@echo "    i2c_pullup -- When using arch i2c driver: configure SDA/SCL pull-ups"
	@echo "        gpio: Use external pull-ups connected to two separate CPU pins. Pull-Ups are always enabled"
	@echo "    kout_nop -- Do not write output to stdout / serial console"
	@echo "    ostream -- include C++ ostream standard library"
	@echo "    trace_malloc -- trace mpmalloc/mpcalloc/mprealloc calls on stdout"
	@echo "    stack_usage -- Generate .su files for stack usage estimation (-> make stack)"
	@echo
	@echo "${arch} drivers:"
	@ls -1 src/arch/${arch}/driver | fgrep .c | cut -d . -f 1 | sed 's/^/    /'
	@echo
	@echo "common drivers:"
	@ls -1 src/driver | fgrep .c | cut -d . -f 1 | sed 's/^/    /'
	@echo
	@echo "available architectures:"
	@ls -1 src/arch | sed 's/^/    /'

info: arch_info
	@echo "COMMON_FLAGS: ${COMMON_FLAGS}"
	@echo "CFLAGS: ${CFLAGS}"
	@echo "CXXFLAGS: ${CXXFLAGS}"
	@echo "Selected (common/arch) drivers: ${drivers} / ${arch_drivers}"

.PHONY: clean default stack stackm help info
