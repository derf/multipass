default: build/system.elf

INCLUDES = -Iinclude
COMMON_FLAGS = -g -Os -Wall -Wextra -fno-rtti
CFLAGS = -std=c99
CXXFLAGS = -std=c++14

TARGETS = src/app/${app}/main.cc src/os/object/cpp_helpers.cc src/os/object/outputstream.cc

include src/app/${app}/Makefile.inc

ifneq ($(findstring lm75,${drivers}), )
	TARGETS += src/driver/lm75.cc
	COMMON_FLAGS += -DDRIVER_LM75
endif

ifneq ($(findstring mmsimple,${drivers}), )
	TARGETS += src/driver/mmsimple.cc
	COMMON_FLAGS += -DDRIVER_MMSIMPLE
endif

ifneq ($(findstring sharp96,${drivers}), )
	TARGETS += src/driver/sharp96.cc
	COMMON_FLAGS += -DDRIVER_SHARP6
	COMMON_FLAGS += -DSHARP96_POWER_PIN=GPIO::${sharp96_power_pin}
	COMMON_FLAGS += -DSHARP96_EN_PIN=GPIO::${sharp96_en_pin}
	COMMON_FLAGS += -DSHARP96_CS_PIN=GPIO::${sharp96_cs_pin}
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

include src/arch/${arch}/Makefile.inc

clean: arch_clean
	rm -f build/system.elf

help: arch_help
	@echo
	@echo "Global flags:"
	@echo "    timer_cycles timer_us timer_s"
	@echo "    loop"
	@echo
	@echo "${arch} drivers:"
	@echo "    $(shell ls src/arch/${arch}/driver | fgrep .c | cut -d . -f 1)"
	@echo
	@echo "Global drivers:"
	@echo "    $(shell ls src/driver | fgrep .c | cut -d . -f 1)"
	@echo
	@echo "Global architectures:"
	@echo "    $(shell ls src/arch)"

.PHONY: clean help
