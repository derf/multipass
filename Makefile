# Only load .config when app/arch are not specified on commandline
ifneq (${app}, )
	CONFIG_app = ${app}
else
	-include .config
endif

ifneq (${arch}, )
	CONFIG_arch = ${arch}
endif

# Make cannot include "foo"/Makefile.inc, so remove quotes from config entries
quote="
app_dir = $(subst ${quote},,${CONFIG_app})
arch_dir = $(subst ${quote},,${CONFIG_arch})

INCLUDES = -Iinclude -Iinclude/arch/${arch_dir}
COMMON_FLAGS = -g -Os -Wall -Wextra
CFLAGS = -std=c99
CXXFLAGS = -std=c++14 -fno-rtti -fno-threadsafe-statics -fno-exceptions

QUIET = @

ASM_TARGETS =
C_TARGETS =
CXX_TARGETS = src/app/${app_dir}/main.cc src/os/object/cpp_helpers.cc src/os/object/outputstream.cc

ifdef verbose
	QUIET =
endif

ifeq (${aspectc}, 1)
	CONFIG_aspectc = y
endif

ifneq (${app_dir}, )
	include src/app/${app_dir}/Makefile.inc
endif

ifneq (${stack_usage}, )
	COMMON_FLAGS += -fstack-usage
endif

# Allow .config driver entries to be overriddenvia commandline / environment

ifneq ($(findstring dummy,${drivers}), )
	CONFIG_driver_dummy = y
endif

ifneq ($(findstring lm75,${drivers}), )
	CONFIG_driver_lm75 = y
endif

ifneq ($(findstring s5851a,${drivers}), )
	CONFIG_driver_s5851a = y
endif

ifneq ($(findstring am2320,${drivers}), )
	CONFIG_driver_am2320 = y
endif

ifneq ($(findstring bme280,${drivers}), )
	CONFIG_driver_bme280 = y
endif

ifneq ($(findstring bme680,${drivers}), )
	CONFIG_driver_bme680 = y
endif

ifneq ($(findstring ccs811,${drivers}), )
	CONFIG_driver_ccs811 = y
endif

ifneq ($(findstring eeprom24lc64,${drivers}), )
	CONFIG_driver_eeprom24lc64 = y
endif

ifneq ($(findstring max44006,${drivers}), )
	CONFIG_driver_max44006 = y
endif

ifneq ($(findstring max44009,${drivers}), )
	CONFIG_driver_max44009 = y
endif

ifneq ($(findstring hdc1080,${drivers}), )
	CONFIG_driver_hdc1080 = y
endif

ifneq ($(findstring mpu9250,${drivers}), )
	CONFIG_driver_mpu9250 = y
endif

ifneq ($(findstring mmsimple,${drivers}), )
	CONFIG_driver_mmsimple = y
endif

ifneq ($(findstring mmsubstate,${drivers}), )
	CONFIG_driver_mmsubstate = y
endif

ifneq ($(findstring nrf24l01,${drivers}), )
	CONFIG_driver_nrf24l01 = y
endif

ifneq ($(findstring sharp96,${drivers}), )
	CONFIG_driver_sharp96 = y
endif

ifneq ($(findstring resistive_load,${drivers}), )
	CONFIG_driver_resistive_load = y
endif

ifneq ($(findstring aemr_transition_sync,${drivers}), )
	CONFIG_driver_aemr_transition_sync = y
endif

ifneq ($(findstring softi2c,${drivers}), )
	CONFIG_driver_softi2c = y
endif

# Load drivers

ifdef CONFIG_driver_dummy
	CXX_TARGETS += src/driver/dummy.cc
	COMMON_FLAGS += -DDRIVER_DUMMY
endif

ifdef CONFIG_driver_lm75
	CXX_TARGETS += src/driver/lm75.cc
	COMMON_FLAGS += -DDRIVER_LM75
endif

ifdef CONFIG_driver_s5851a
	CXX_TARGETS += src/driver/s5851a.cc
	COMMON_FLAGS += -DDRIVER_S5851A
endif

ifdef CONFIG_driver_am2320
	CXX_TARGETS += src/driver/am2320.cc
	COMMON_FLAGS += -DDRIVER_AM2320
endif

ifdef CONFIG_driver_bme280
	CXX_TARGETS += src/driver/bme280.cc src/driver/bme680_util.cc
	COMMON_FLAGS += -DDRIVER_BME280
endif

ifdef CONFIG_driver_bme680
	CXX_TARGETS += src/driver/bme680.cc src/driver/bme680_util.cc
	COMMON_FLAGS += -DDRIVER_BME680
endif

ifdef CONFIG_driver_ccs811
	CXX_TARGETS += src/driver/ccs811.cc
	COMMON_FLAGS += -DDRIVER_CCS811
endif

ifdef CONFIG_driver_eeprom24lc64
	CXX_TARGETS += src/driver/eeprom24lc64.cc
	COMMON_FLAGS += -DDRIVER_EEPROM24LC64
endif

ifdef CONFIG_driver_max44006
	CXX_TARGETS += src/driver/max44006.cc
	COMMON_FLAGS += -DDRIVER_MAX44006
endif

ifdef CONFIG_driver_max44009
	CXX_TARGETS += src/driver/max44009.cc
	COMMON_FLAGS += -DDRIVER_MAX44009
endif

ifdef CONFIG_driver_hdc1080
	CXX_TARGETS += src/driver/hdc1080.cc
	COMMON_FLAGS += -DDRIVER_HDC1080
endif

ifdef CONFIG_driver_mpu9250
	CXX_TARGETS += src/driver/mpu9250.cc
	COMMON_FLAGS += -DDRIVER_MPU9250
endif

ifdef CONFIG_driver_mmsimple
	CXX_TARGETS += src/driver/mmsimple.cc
	COMMON_FLAGS += -DDRIVER_MMSIMPLE
endif

ifdef CONFIG_driver_mmsubstate
	CXX_TARGETS += src/driver/mmsubstate.cc
	COMMON_FLAGS += -DDRIVER_MMSUBSTATE
endif

ifdef CONFIG_driver_nrf24l01
	CXX_TARGETS += src/driver/nrf24l01.cc
	ifeq (${arch_dir}, msp430fr5994lp)
		nrf24l01_cs_pin ?= p1_3
		nrf24l01_en_pin ?= p6_2
		nrf24l01_irq_pin ?= p8_3
	endif
	ifeq (${arch_dir}, msp430fr5969lp)
		nrf24l01_en_pin ?= p4_3
		nrf24l01_cs_pin ?= p2_4
		nrf24l01_irq_pin ?= p1_5
	endif
	ifeq (${arch_dir}, arduino-nano)
		nrf24l01_en_pin ?= pc1
		nrf24l01_cs_pin ?= pc0
		nrf24l01_irq_pin ?= pc2
	endif
	COMMON_FLAGS += -DDRIVER_NRF24L01
	COMMON_FLAGS += -DNRF24L01_EN_PIN=GPIO::${nrf24l01_en_pin}
	COMMON_FLAGS += -DNRF24L01_CS_PIN=GPIO::${nrf24l01_cs_pin}
	COMMON_FLAGS += -DNRF24L01_IRQ_PIN=GPIO::${nrf24l01_irq_pin}
endif

ifdef CONFIG_driver_sharp96
	CXX_TARGETS += src/driver/sharp96.cc
	ifeq (${arch_dir}, msp430fr5969lp)
		sharp96_power_pin ?= p4_2
		sharp96_en_pin ?= p4_3
		sharp96_cs_pin ?= p2_4
	endif
	ifeq (${arch_dir}, msp430fr5994lp)
		sharp96_power_pin ?= p1_2
		sharp96_en_pin ?= p6_2
		sharp96_cs_pin ?= p1_3
	endif
	COMMON_FLAGS += -DDRIVER_SHARP6
	COMMON_FLAGS += -DSHARP96_POWER_PIN=GPIO::${sharp96_power_pin}
	COMMON_FLAGS += -DSHARP96_EN_PIN=GPIO::${sharp96_en_pin}
	COMMON_FLAGS += -DSHARP96_CS_PIN=GPIO::${sharp96_cs_pin}
endif

ifdef CONFIG_driver_resistive_load
	CXX_TARGETS += src/driver/resistive_load.cc
	resistor1_pin ?= p3_0
	resistor2_pin ?= p3_1
	resistor3_pin ?= p3_2
	resistor4_pin ?= p3_3
	COMMON_FLAGS += -DDRIVER_RESISTIVE_LOAD
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN1=GPIO::${resistor1_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN2=GPIO::${resistor2_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN3=GPIO::${resistor3_pin}
	COMMON_FLAGS += -DRESISTIVE_LOAD_PIN4=GPIO::${resistor4_pin}
endif

ifdef CONFIG_driver_aemr_transition_sync
	CXX_TARGETS += src/driver/aemr_transition_sync.cc
endif

ifdef CONFIG_driver_softi2c
	CXX_TARGETS += src/driver/soft_i2c.cc
	COMMON_FLAGS += -DDRIVER_SOFTI2C
endif

# Configure drivers (TODO: Kconfig)

ifneq (${i2c_freq}, )
	CONFIG_i2c_freq = ${i2c_freq}
endif
ifdef CONFIG_i2c_freq
	COMMON_FLAGS += -DF_I2C=${CONFIG_i2c_freq}
endif

ifneq (${timer_freq}, )
	CONFIG_timer_freq = ${timer_freq}
endif
ifdef CONFIG_timer_freq
	COMMON_FLAGS += -DF_TIMER=${CONFIG_timer_freq}
endif

ifdef CONFIG_driver_softi2c_pullup-dynamic_internal
	softi2c_pullup=internal
endif
ifeq (${softi2c_pullup}, internal)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_INTERNAL
endif

ifdef CONFIG_driver_softi2c_pullup_dynamic_external
	softi2c_pullup=external
endif
ifeq (${softi2c_pullup}, external)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_EXTERNAL
endif

ifdef CONFIG_driver_softi2c_pullup_external
	softi2c_pullup=gpio
endif
ifeq (${softi2c_pullup}, gpio)
	COMMON_FLAGS += -DSOFTI2C_PULLUP_FIXED_GPIO
endif

ifeq (${i2c_pullup}, gpio)
	COMMON_FLAGS += -DI2C_PULLUP_FIXED_GPIO
endif

ifeq (${softi2c_timer}, 1)
	override arch_drivers += ,timer
	COMMON_FLAGS += -DSOFTI2C_TIMER
endif

ifeq (${timer_us}, 1)
	COMMON_FLAGS += -DTIMER_US
endif

ifeq (${loop}, 1)
	COMMON_FLAGS += -DWITH_LOOP
endif
ifdef CONFIG_loop
	COMMON_FLAGS += -DWITH_LOOP
endif

ifeq (${wakeup}, 1)
	COMMON_FLAGS += -DWITH_WAKEUP
endif
ifdef CONFIG_wakeup
	COMMON_FLAGS += -DWITH_WAKEUP
endif

ifeq (${ostream}, 1)
	COMMON_FLAGS += -DWITH_OSTREAM
endif
ifdef CONFIG_ostream
	COMMON_FLAGS += -DWITH_OSTREAM
endif

ifeq (${trace_malloc}, 1)
	CXX_TARGETS += src/lib/mpmalloc.cc
	COMMON_FLAGS += -DMULTIPASS_TRACE_MALLOC
endif

ifneq (${arch_dir}, )
	include src/arch/${arch_dir}/Makefile.inc
endif

default: build/system.elf

stack: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

stackm: default
	${QUIET}test -n "${OBJDUMP}"
	${QUIET}test -n "${ARCH_SHORTNAME}"
	${QUIET}script/static-stack-analyze.pl --machine-readable ${OBJDUMP} ${ARCH_SHORTNAME} ${OBJECTS}

Kconfig: script/mkconfig
	${QUIET}script/mkconfig > Kconfig

config: Kconfig
	${QUIET}kconfig-qconf Kconfig

randconfig: Kconfig
	${QUIET}script/randconfig

clean: arch_clean
	${QUIET}rm -f build/system.elf

help: arch_help
	@echo
	@echo "common flags:"
	@echo "    timer_us timer_s -- enable specific timers. May be mutually exclusive"
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
	@echo "${arch_dir} drivers:"
	@ls -1 src/arch/${arch_dir}/driver | fgrep .c | cut -d . -f 1 | sed 's/^/    /'
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

.PHONY: clean config default randconfig stack stackm help info
