# vim:ft=make

CC = gcc
CXX = g++

INCLUDES += -Iinclude/posix
TARGETS += src/arch/posix/arch.cc src/arch/posix/driver/gpio.cc
TARGETS += src/arch/posix/driver/stdout.cc

OBJECTS = ${TARGETS:.cc=.o}

.cc.o:
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

build/system.elf: ${OBJECTS}
	${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-o $@ ${OBJECTS}

run: build/system.elf
	build/system.elf

arch_clean:
	rm -f ${OBJECTS}

.PHONY: arch_clean program
