ifeq (${prototest_bench_energy}, 1)
	loop ?= 1
	COMMON_FLAGS += -DPROTOTEST_BENCH_ENERGY
endif

ifeq (${prototest_bench_cycles}, 1)
	loop ?= 0
	arch_drivers += ,counter
	COMMON_FLAGS += -DPROTOTEST_BENCH_CYCLES
endif

ifneq (${prototest_include_global}, )
	COMMON_FLAGS += -DPROTOTEST_INCLUDE_GLOBAL=${prototest_include_global}
endif

ifneq (${prototest_include_local}, )
	COMMON_FLAGS += -DPROTOTEST_INCLUDE_LOCAL=${prototest_include_local}
endif

ifeq (${prototest_arduinojson}, 1)
	COMMON_FLAGS += -DPROTOTEST_ARDUINOJSON
endif

ifeq (${prototest_capnproto_c}, 1)
	COMMON_FLAGS += -DPROTOTEST_CAPNPROTO_C
	CXX_TARGETS += src/app/prototest/capnp_c_bench.capnp.cc
	CXX_TARGETS += src/lib/capnp-c/capn.cc
	CXX_TARGETS += src/lib/capnp-c/capn-malloc.cc
	CXX_TARGETS += src/lib/capnp-c/capn-stream.cc
	INCLUDES += -Iinclude/lib/capnp-c
endif

ifeq (${prototest_manualjson}, 1)
	COMMON_FLAGS += -DPROTOTEST_MANUALJSON
endif

ifeq (${prototest_modernjson}, 1)
	COMMON_FLAGS += -DPROTOTEST_MODERNJSON
	ostream = 1
endif

ifeq (${prototest_mpack}, 1)
	COMMON_FLAGS += -DPROTOTEST_MPACK
	CXX_TARGETS += src/lib/mpack/mpack.cc
	INCLUDES += -Iinclude/lib/mpack
endif

ifeq (${prototest_nanopb}, 1)
	COMMON_FLAGS += -DPROTOTEST_NANOPB
	CXX_TARGETS += src/app/prototest/nanopbbench.pb.cc src/lib/nanopb/pb_common.cc
	CXX_TARGETS += src/lib/nanopb/pb_decode.cc src/lib/nanopb/pb_encode.cc
	INCLUDES += -Iinclude/lib/nanopb
endif

ifeq (${prototest_ubjson}, 1)
	COMMON_FLAGS += -DPROTOTEST_UBJSON
	C_TARGETS += src/lib/ubjson/ubjr.c src/lib/ubjson/ubjw.c
	INCLUDES += -Iinclude/lib/ubjson
endif

ifeq (${prototest_xdr}, 1)
	COMMON_FLAGS += -DPROTOTEST_XDR
	CXX_TARGETS += src/os/object/xdrstream.cc src/os/object/xdrinput.cc
endif

# Don't try to make .capnp from .capnp.c
%.capnp: ;

# Don't try to make .proto from .proto.c
%.proto: ;

%.pb.cc: %.proto
	protoc --plugin=protoc-gen-nanopb=${HOME}/var/ess/protocol-modeling/nanopb/generator/protoc-gen-nanopb --nanopb_out=. src/app/prototest/nanopbbench.proto
	mv src/app/prototest/nanopbbench.pb.c src/app/prototest/nanopbbench.pb.cc
	sed -i 's!src/app/prototest/!!' src/app/prototest/nanopbbench.pb.cc
	sed -i 's!\#include "src/app/prototest/nanopb.pb.h"!!' src/app/prototest/nanopbbench.pb.h

%.capnp.cc: %.capnp
	capnp compile -oc $<
	mv $<.c $<.cc
	cp $<.cc /tmp
