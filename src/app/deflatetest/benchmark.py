#!/usr/bin/env python3

import numpy as np
import re
import subprocess
import sys


def main(arch):
    make_args = [f"arch={arch}", "app=deflatetest"]
    base_rom = None
    base_ram = None
    defl_rom = None
    defl_ram = None
    defl_ms = None
    defl_size = None

    status = subprocess.run(
        ["./mp"] + make_args + ["deflate_nop=1"],
        stdout=subprocess.PIPE,
        universal_newlines=True,
    )

    for line in status.stdout.split("\n"):
        match = re.search(r"ROM: (\d+) .* RAM: (\d+)", line)
        if match:
            base_rom = int(match.group(1))
            base_ram = int(match.group(2))

    status = subprocess.run(
        ["./mp"] + make_args, stdout=subprocess.PIPE, universal_newlines=True
    )

    for line in status.stdout.split("\n"):
        match = re.search(r"ROM: (\d+) .* RAM: (\d+)", line)
        if match:
            defl_rom = int(match.group(1))
            defl_ram = int(match.group(2))

    rom_usage = int(np.ceil((defl_rom - base_rom) / 16)) * 16
    ram_usage = int(np.ceil((defl_ram - base_ram) / 16)) * 16
    print(f"| {arch} | {rom_usage} B | {ram_usage} B")

    status = subprocess.run(
        ["make", "cat"] + make_args, stdout=subprocess.PIPE, universal_newlines=True
    )

    for line in status.stdout.split("\n"):
        match = re.match(r"took ([0-9.]+) ms", line)
        if match:
            defl_ms = float(match.group(1))
        match = re.match(r"inflate returned (\d+)", line)
        if match:
            defl_size = int(match.group(1))

    defl_speed = defl_size / defl_ms
    print(f"{defl_speed:3.0f} kB/s    ({defl_size:4d} B @ {defl_ms:6.2f} ms)")


if __name__ == "__main__":
    main(*sys.argv[1:])
