#!/usr/bin/env python3

import PIL
import PIL.ImageDraw
import PIL.ImageFont

def invert(x):
    ret = 0
    for i in range(8):
        if x & (0x80 >> i):
            ret |= 0x01 << i
    return ret


if __name__ == "__main__":
    font = PIL.ImageFont.truetype(
        "/usr/share/fonts/truetype/terminus/TerminusTTF-4.46.0.ttf", size=14
    )

    for char in range(0x20, 0x7F):
        fontimg = PIL.Image.new("1", (16, 16))
        draw = PIL.ImageDraw.Draw(fontimg)
        draw.text((0, 0), chr(char), font=font, fill=255)

        bbox = fontimg.getbbox()

        if bbox is None:
            print(
                "glyph_line_t GLYPH_ATTR chr16_032[] = {0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // <spac>"
            )
            continue

        left, upper, right, lower = bbox
        fontimg = fontimg.crop((left, 0, right, 15))

        fontimg = fontimg.transpose(PIL.Image.ROTATE_270).transpose(PIL.Image.FLIP_LEFT_RIGHT)
        fontbytes = fontimg.tobytes()

        glyph_len = len(fontbytes)
        glyph_data = "{" + f"0x{glyph_len:02x}," + ",".join(map(lambda x: f"0x{invert(x):02x}", fontbytes)) + "}"

        assert glyph_len % 2 == 0

        print(
            f"glyph_line_t GLYPH_ATTR chr16_{char:03d}[] = {glyph_data}; // {chr(char)}"
        )
