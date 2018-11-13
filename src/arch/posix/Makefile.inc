# vim:ft=make

COMMON_FLAGS += -DMULTIPASS_ARCH_posix

CC = gcc
CXX = g++

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 -p . --Xcompiler
endif

CXX_TARGETS += src/arch/posix/arch.cc src/arch/posix/driver/gpio.cc
CXX_TARGETS += src/arch/posix/driver/stdout.cc src/arch/posix/driver/uptime.cc

OBJECTS = ${CXX_TARGETS:.cc=.o}

ifneq (${gpio_trace}, )
	COMMON_FLAGS += -DGPIO_TRACE
endif

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

arch_info:

.PHONY: arch_clean arch_help arch_info monitor program run
