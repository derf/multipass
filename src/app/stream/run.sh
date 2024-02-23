#!/bin/sh

cd "$(git rev-parse --show-toplevel)"

target="$(grep '^CONFIG_arch=' .config | cut -d '"' -f 2)"

mkdir -p src/app/stream/log
: > src/app/stream/log/${target}.txt

for stride in 1 2 4 8 16 32 64; do
	for type in uint8_t uint16_t uint32_t uint64_t float double; do
		kconfig-tweak --keep-case --set-str app_stream_stride $stride
		kconfig-tweak --keep-case --set-str app_stream_type $type

		echo
		echo "stride: ${stride}"
		echo "type  : ${type}"
		echo

		./mp && make cat >> src/app/stream/log/${target}.txt
	done
done
