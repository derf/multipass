#!/usr/bin/env python3

import PIL
import PIL.ImageDraw
import PIL.ImageFont

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
                "glyph_line_t GLYPH_ATTR chr_032[] = {0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // <spac>"
            )
            continue

        left, upper, right, lower = bbox
        fontimg = fontimg.crop((left, 0, right, 15))

        fontimg = fontimg.transpose(PIL.Image.ROTATE_270)
        fontbytes = fontimg.tobytes()

        glyph_len = len(fontbytes)
        glyph_data = "{" + ",".join(map(lambda x: f"0x{x:02x}", fontbytes)) + "}"

        print(
            f"glyph_line_t GLYPH_ATTR chr_{char:03d}[] = {glyph_data}; // {chr(char)}"
        )
