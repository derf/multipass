#!/bin/sh

set -e

for i in info program; do
	make -B "$@" $i
done
