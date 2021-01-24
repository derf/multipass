#!/usr/bin/env python3

import sys
import zlib

level = -1
if len(sys.argv) > 2:
    level = int(sys.argv[2])

input_data = sys.argv[1].encode("utf-8")

try:
    with open(sys.argv[1], "rb") as f:
        input_data = f.read()
except FileNotFoundError:
    input_data = sys.argv[1].encode("utf-8")

output = zlib.compress(input_data, level=level)

cinfo = (output[0] & 0xF0) >> 4
window_size = 1 << (8 + cinfo)
print(f"// CINFO = {cinfo}  (window size = {window_size})")

fdict = output[1] & 0x20
if fdict:
    print("// FDICT = 1 (preset dictionary in use)")
else:
    print("// FDICT = 0 (no preset dictionary)")

is_final = output[2] & 0x01
block_type = (output[2] & 0x06) >> 1

print(f"// first Huffman block: type={block_type} is_final={is_final}")

print(f"// {sys.argv[1]}")
print("unsigned char const inflate_input[] = {" + ", ".join(map(str, output)) + "};")
print(f"unsigned short const inflate_input_size = {len(output)};")
print(f"unsigned char deflate_output[{len(input_data) + 1}];")
print(f"unsigned short const deflate_output_size = {len(input_data) + 1};")
