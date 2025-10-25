#!/usr/bin/env python3
#
# Copyright 2020 Birte Kristina Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

import json
import re
import subprocess
import sys


def main(size_executable, rom_sections, ram_sections):
    rom_sections = rom_sections.split(",")
    ram_sections = ram_sections.split(",")

    status = subprocess.run(
        size_executable.split() + ["-A", "build/system.elf"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
    )

    section_size = dict()

    for line in status.stdout.split("\n"):
        match = re.match(r"[.](\S+)\s+(\d+)", line)
        if match:
            section = match.group(1)
            size = int(match.group(2))
            section_size[section] = size

    total = {
        "ROM": sum(map(lambda section: section_size.get(section, 0), rom_sections)),
        "RAM": sum(map(lambda section: section_size.get(section, 0), ram_sections)),
    }

    output = {"section": section_size, "total": total}

    print(json.dumps(output))


if __name__ == "__main__":
    main(*sys.argv[1:])
