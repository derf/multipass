#!/usr/bin/env python3
#
# Copyright 2021 Birte Kristina Friesel
#
# SPDX-License-Identifier: BSD-2-Clause

import json
import re
import subprocess
import sys


def main(size_executable, rom_sections, ram_sections, file="build/system.elf"):
    glob = False
    if "*" in rom_sections or "*" in ram_sections:
        glob = True

    rom_sections = rom_sections.split(",")
    ram_sections = ram_sections.split(",")

    status = subprocess.run(
        size_executable.split() + ["-A", file],
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

    if glob:
        total = {"ROM": 0, "RAM": 0}
        for rom_section in rom_sections:
            for section, sec_size in section_size.items():
                if (
                    rom_section == section
                    or rom_section.endswith("*")
                    and section.startswith(rom_section.removesuffix("*"))
                ):
                    total["ROM"] += sec_size
        for ram_section in ram_sections:
            for section, sec_size in section_size.items():
                if (
                    ram_section == section
                    or ram_section.endswith("*")
                    and section.startswith(ram_section.removesuffix("*"))
                ):
                    total["RAM"] += sec_size
    else:
        total = {
            "ROM": sum(map(lambda section: section_size.get(section, 0), rom_sections)),
            "RAM": sum(map(lambda section: section_size.get(section, 0), ram_sections)),
        }

    output = {file: total}

    print(json.dumps(output))


if __name__ == "__main__":
    main(*sys.argv[1:])
