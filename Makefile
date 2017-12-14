default: build/system.elf

INCLUDES = -Iinclude
COMMON_FLAGS = -g -Os -Wall -Wextra -unused -fno-rtti
CFLAGS = -std=c99
CXXFLAGS = -std=c++14

TARGETS = src/app/${app}/main.cc src/os/object/cpp_helpers.cc src/os/object/outputstream.cc
TARGETS += src/driver/lm75.cc

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

.PHONY: clean
