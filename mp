#!/bin/sh
#
# Copyright 2020 Daniel Friesel
#
# SPDX-License-Identifier: CC0-1.0

set -e

find src -name '*.o' -delete
rm -f build/system.elf

for i in info program; do
	make "$@" $i
done
