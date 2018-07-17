# vim:ft=make

CC = gcc
CXX = g++

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 -p . --Xcompiler
endif

TARGETS += src/arch/posix/arch.cc src/arch/posix/driver/gpio.cc
TARGETS += src/arch/posix/driver/stdout.cc src/arch/posix/driver/uptime.cc

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-o $@ ${OBJECTS}

run: build/system.elf
	build/system.elf

monitor: run

program: run

arch_clean:
	rm -f ${OBJECTS}

arch_help:
	@echo "posix specific flags:"
	@echo "    - none -"

.PHONY: arch_clean arch_help monitor program run
