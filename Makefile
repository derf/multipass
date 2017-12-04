default: build/system.elf

INCLUDES = -Iinclude
COMMON_FLAGS = -g -Os -Wall -Wextra -unused
CFLAGS = -std=c99
CXXFLAGS = -std=c++14

TARGETS = src/os/main.cc

ifeq (${arch}, msp430fr5969lp)
include src/arch/msp430fr5969lp/Makefile.inc
endif

ifeq (${arch}, posix)
include src/arch/posix/Makefile.inc
endif

clean: arch_clean
	rm -f build/system.elf

.PHONY: clean
