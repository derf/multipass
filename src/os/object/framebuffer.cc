#include "object/framebuffer.h"

#ifdef CONFIG_framebuffer_in_text_segment
__attribute__ ((section(".text")))
#endif
static unsigned char framebuffer[(unsigned long int)CONFIG_framebuffer_width * (unsigned long int)CONFIG_framebuffer_height / 8];

void Framebuffer::clear()
{
	for (unsigned int i = 0; i < width * (height / 8); i++) {
		data[i] = 0;
	}
}

void Framebuffer::fillBox(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	if (w == 0 || h == 0) {
		return;
	}
	w -= 1;
	h -= 1;
	if ((x+w) >= width || (y+h) >= height) {
		return;
	}

	unsigned int x1 = (height/8)*x;
	unsigned int x2 = (height/8)*(x+w);
	unsigned int y2 = height-1-y;
	unsigned int y1 = y2-h;
	unsigned char y2_mask = 0xff << (8 - (y2 % 8)); // bits from 0 to y2%8 must be filled
	unsigned char y1_mask = 0xff >> (y1 % 8); // bits from y1%8 to 7 must be filled
	y1 = y1/8;
	y2 = y2/8;

	if (y1 == y2) {
		// y1_mask and y2_mask overlap
		for (unsigned int pos_x = x1; pos_x < x2; pos_x += height/8) {
			data[pos_x + y1] |= y1_mask & y2_mask;
		}
	} else {
		if (y1_mask != 0xff) {
			for (unsigned int pos_x = x1; pos_x < x2; pos_x += height/8) {
				data[pos_x + y1] |= y1_mask;
			}
			y1++;
		}
		for (unsigned int pos_x = x1; pos_x < x2; pos_x += height/8) {
			for (unsigned int pos_y = y1; pos_y < y2; pos_y++) {
				data[pos_x + pos_y] = 0xff;
			}
		}
		if (y2_mask) {
			for (unsigned int pos_x = x1; pos_x < x2; pos_x += height/8) {
				data[pos_x + y2] |= y2_mask;
			}
		}
	}
}

void Framebuffer::drawAt(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *image)
{
	y /= 8;
	for (unsigned int pos_x = 0; pos_x < w; pos_x++) {
		for (unsigned int pos_y = 0; pos_y < h/8; pos_y++) {
			data[(x + pos_x) * (height/8) + y + pos_y] = image[pos_x * (h/8) + pos_y];
		}
	}
}

void Framebuffer::put(char c)
{
	if (font == 0) {
		return;
	}
	if (c == '\n') {
		fontX = 0;
		fontY += 8;
		return;
	}
	if ((c < 32) || (c > 126)) {
		c = '?';
	}
	glyph_t glyph = font[c - 32];
	const unsigned char glyph_w = glyph[0];

	if (fontX + glyph_w + 1 >= width) {
		put('\n');
	}
	if (fontY >= height) {
		return;
	}
	for (unsigned char i = 0; i < glyph_w; i++) {
		data[(height/8) * (fontX + i) + fontY/8] = glyph[i+1];
	}
	data[(height/8) * (fontX + glyph_w + 1) + fontY/8] = 0;
	fontX += glyph_w + 2;
}

Framebuffer fb(framebuffer);
