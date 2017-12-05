default: build/system.elf

INCLUDES = -Iinclude
COMMON_FLAGS = -g -Os -Wall -Wextra -unused
CFLAGS = -std=c99
CXXFLAGS = -std=c++14

TARGETS = src/os/main.cc

include src/arch/${arch}/Makefile.inc

clean: arch_clean
	rm -f build/system.elf

.PHONY: clean
