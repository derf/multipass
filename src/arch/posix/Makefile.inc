# vim:ft=make

COMMON_FLAGS += -DMULTIPASS_ARCH_posix

CC = gcc
CXX = g++
OBJDUMP = objdump

ARCH_SHORTNAME = x64

ifeq (${aspectc}, 1)
	CXX = ag++ -r build/repo.acp -v 0 -p . --Xcompiler
endif

CXX_TARGETS += src/arch/posix/arch.cc src/arch/posix/driver/gpio.cc
CXX_TARGETS += src/arch/posix/driver/stdout.cc src/arch/posix/driver/uptime.cc

ifneq ($(findstring counter,${arch_drivers}), )
	CXX_TARGETS += src/arch/posix/driver/counter.cc
endif

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o}

ifneq (${gpio_trace}, )
	COMMON_FLAGS += -DGPIO_TRACE
endif

.cc.o:
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

.c.o:
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

build/system.elf: ${OBJECTS}
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-o $@ ${OBJECTS}

run: build/system.elf
	build/system.elf

monitor: run

program:

arch_clean:
	rm -f ${OBJECTS}

arch_help:
	@echo "posix specific flags:"
	@echo "    - none -"

arch_info:
	@echo "Monitor: run build/system.elf"

.PHONY: arch_clean arch_help arch_info monitor program run
