# vim:ft=make
#
# Copyright 2022 Daniel Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

CPU = tc1796

cpu_freq ?= 150000000
counter_freq ?= 75000000
uart_freq ?= 115200
SERIAL_PORT ?= ttyUSB0
QEMU_PORT ?= 12345

COMMON_FLAGS += -mcpu=${CPU} -DMULTIPASS_ARCH_tc1796

ARCH_SHORTNAME = tc1796

LICENSE = -mlicense-dir=${HOME}/var/source/aurix-infineon-hightec-tricore
CC =      wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-gcc.exe ${LICENSE}
CXX =     wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-g++.exe ${LICENSE}
OBJCOPY = wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-objcopy.exe
OBJDUMP = wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-objdump.exe ${LICENSE}
SIZE =    wine ${HOME}/.wine/drive_c/HighTec/toolchains/tricore/v4.9.3.0-infineon-1.0/bin/tricore-size.exe

CXX_TARGETS += src/arch/tc1796-triboard/arch.cc
CXX_TARGETS += src/arch/tc1796-triboard/driver/gpio.cc
CXX_TARGETS += src/arch/tc1796-triboard/driver/stdout.cc

ifneq (${cpu_freq}, )
	COMMON_FLAGS += -DF_CPU=${cpu_freq}U
else
	COMMON_FLAGS += -DF_CPU=150000000U
endif

ifneq ($(findstring counter,${arch_drivers}), )
	CONFIG_arch_tc1796_triboard_driver_counter = y
endif

ifdef CONFIG_arch_tc1796_triboard_driver_counter
	CXX_TARGETS += src/arch/tc1796-triboard/driver/counter.cc
endif

OBJECTS = ${CXX_TARGETS:.cc=.o} ${C_TARGETS:.c=.o} ${ASM_TARGETS:.S=.o}

%.o : %.cc | include/config.h
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} -c -o $@ ${@:.o=.cc}

%.o : %.c | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} ${CFLAGS} -c -o $@ ${@:.o=.c}

%.o : %.S | include/config.h
	${QUIET}${CC} ${INCLUDES} ${COMMON_FLAGS} -Wa,-gstabs,-ggdb -x assembler-with-cpp -c -o $@ ${@:.o=.S}

build/system.elf: ${OBJECTS}
	${QUIET}mkdir -p build
	${QUIET}${CXX} ${INCLUDES} ${COMMON_FLAGS} ${CXXFLAGS} \
		-Wl,--gc-sections \
		-o $@ ${OBJECTS}

build/system.bin: build/system.elf
	${QUIET}${OBJCOPY} -O binary $< $@

# Needs a Debian3.1 VM with ios:/ess/dortmund/proj/compiler/tricore-gcc-3.4.5 installed to /opt/trigcc345,
# http://archive.debian.org/debian-security/pool/updates/main/r/resmgr/libresmgr1_1.0-2sarge2_i386.deb http://archive.debian.org/debian-security/pool/updates/main/r/resmgr/resmgr_1.0-2sarge2_i386.deb installed,
# and SSH via localhost:2022. E.g.:
# > qemu-system-i386 -boot c -m 2048 -hda debian3-1.qcow2 -machine type=pc,accel=kvm:tcg --enable-kvm -net nic,model=ne2k_pci -net user,hostfwd=tcp::2022-:22 -smp cores=1,threads=2 -usb -monitor telnet::${QEMU_PORT},server,nowait
# Must already be running in the background before 'make program' can be used.
program: build/system.bin
	${QUIET}SSH_ASKPASS=/home/derf/var/projects/multipass/src/arch/tc1796-triboard/ssh-askpass SSH_ASKPASS_REQUIRE=force ssh -oKexAlgorithms=+diffie-hellman-group-exchange-sha1,diffie-hellman-group1-sha1 -oHostKeyAlgorithms=+ssh-rsa -p 2022 root@localhost  true
	${QUIET}echo 'device_add usb-host,vendorid=0x058b,productid=0x0028,id=tricore' | nc -q 1 localhost ${QEMU_PORT} > /dev/null
	${QUIET}SSH_ASKPASS=/home/derf/var/projects/multipass/src/arch/tc1796-triboard/ssh-askpass SSH_ASKPASS_REQUIRE=force scp -oKexAlgorithms=+diffie-hellman-group-exchange-sha1,diffie-hellman-group1-sha1 -oHostKeyAlgorithms=+ssh-rsa -P 2022 build/system.bin root@localhost:/tmp/system.bin
	${QUIET}SSH_ASKPASS=/home/derf/var/projects/multipass/src/arch/tc1796-triboard/ssh-askpass SSH_ASKPASS_REQUIRE=force ssh -oKexAlgorithms=+diffie-hellman-group-exchange-sha1,diffie-hellman-group1-sha1 -oHostKeyAlgorithms=+ssh-rsa -p 2022 root@localhost "/opt/trigcc345/bin/tricore-jtag-usb-flash /opt/trigcc345/tricore/flash_driver/intern/tc1796/flashprg.elf /tmp/system.bin"
	${QUIET}echo 'device_del tricore' | nc -q 1 localhost ${QEMU_PORT} > /dev/null

monitor:
	${QUIET}screen /dev/${SERIAL_PORT} ${uart_freq}

arch_clean:
	${QUIET}rm -f ${OBJECTS} build/system.elf build/system.bin

arch_help:
	@true

arch_info:
	@echo "CPU   Freq: ${cpu_freq} Hz"
	@echo "Count Freq: ${counter_freq} Hz"
	@echo "Counter Overflow: 4294967296/4294967295"
	@echo "Monitor: /dev/${SERIAL_PORT} ${uart_freq}"

attributes: build/system.elf
	${QUIET}script/size.py "${SIZE}" text,rodata bss

nfpvalues: build/system.elf
	${QUIET}script/nfpvalues.py "${SIZE}" text,rodata bss

.PHONY: arch_clean arch_help arch_info attributes monitor program nfpvalues
