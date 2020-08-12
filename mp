#!/bin/sh

set -e

find src -name '*.o' -delete

for i in info program; do
	make "$@" $i
done
