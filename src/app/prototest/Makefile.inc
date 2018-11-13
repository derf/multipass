loop ?= 1

ifeq (${prototest_arduinojson}, 1)
	COMMON_FLAGS += -DPROTOTEST_ARDUINOJSON
endif

ifeq (${prototest_modernjson}, 1)
	COMMON_FLAGS += -DPROTOTEST_MODERNJSON
endif

ifeq (${prototest_mpack}, 1)
	COMMON_FLAGS += -DPROTOTEST_MPACK
	TARGETS += src/lib/mpack/mpack.cc
	INCLUDES += -Iinclude/lib/mpack
endif

ifeq (${prototest_nanopb}, 1)
	COMMON_FLAGS += -DPROTOTEST_NANOPB
	TARGETS += src/app/prototest/nanopb.pb.cc src/lib/nanopb/pb_common.cc
	TARGETS += src/lib/nanopb/pb_decode.cc src/lib/nanopb/pb_encode.cc
	INCLUDES += -Iinclude/lib/nanopb
endif

ifeq (${prototest_xdr}, 1)
	COMMON_FLAGS += -DPROTOTEST_XDR
	TARGETS += src/os/object/xdrstream.cc src/os/object/xdrinput.cc
endif

%.pb.cc: %.proto
	protoc --plugin=protoc-gen-nanopb=${HOME}/var/ess/protocol-modeling/nanopb/generator/protoc-gen-nanopb --nanopb_out=. src/app/prototest/nanopb.proto
	mv src/app/prototest/nanopb.pb.c src/app/prototest/nanopb.pb.cc
	sed -i 's!src/app/prototest/!!' src/app/prototest/nanopb.pb.cc
