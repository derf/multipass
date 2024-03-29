#include "object/framebuffer.h"

#ifdef MULTIPASS_ARCH_arduino_nano
#include <avr/pgmspace.h>
#endif

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

void Framebuffer::drawBattery(unsigned int x, unsigned int y, unsigned char percent, bool charging)
{
	for (unsigned char i = 0; i < 13; i++) {
		data[(x+i) * (height/8) + y] = 0x81 | (0xff * (percent*2 >= i*15));
	}
	data[(x+11) * (height/8) + y/8] |= 0xe7;
	data[(x+12) * (height/8) + y/8] &= ~0x81;
	data[(x+12) * (height/8) + y/8] |= 0x24;
	data[(x+13) * (height/8) + y/8] = 0x3c;

	if (charging) {
		data[(x+2) * (height/8) + y/8] ^= 0x7e;
		data[(x+3) * (height/8) + y/8] ^= 0x3c;
		data[(x+4) * (height/8) + y/8] ^= 0x18;
		data[(x+7) * (height/8) + y/8] ^= 0x7e;
		data[(x+8) * (height/8) + y/8] ^= 0x3c;
		data[(x+9) * (height/8) + y/8] ^= 0x18;
	}
}

void Framebuffer::scroll()
{
	for (unsigned int pos_x = 0; pos_x < width; pos_x++) {
		for (unsigned int pos_y = fontSize; pos_y < height/8; pos_y++) {
			data[pos_x * (height/8) + pos_y - fontSize] = data[pos_x * (height/8) + pos_y];
		}
		for (unsigned int pos_y = 1; pos_y <= fontSize; pos_y++) {
			data[pos_x * (height/8) + height/8 - pos_y] = 0;
		}
	}
	if (fontY >= 8*fontSize) {
		fontY -= 8*fontSize;
	}
}

void Framebuffer::put(char c)
{
	if (font == 0) {
		return;
	}
	if (c == '\n') {
		fontX = 0;
		fontY += 8*fontSize;
		return;
	}
	if ((c < 32) || (c > 126)) {
		c = '?';
	}
#ifdef MULTIPASS_ARCH_arduino_nano
	uint8_t *glyph_addr = (uint8_t *)pgm_read_ptr(&font[c - 32]);
	const unsigned char glyph_w = pgm_read_byte(&glyph_addr[0]);
#else
	glyph_t glyph = font[c - 32];
	const unsigned char glyph_w = glyph[0];
#endif

	if (fontX + glyph_w + 1 >= width) {
		put('\n');
	}
	if (fontY + 8*fontSize > height) {
		scroll();
	}
	for (unsigned char i = 0; i < glyph_w; i++) {
		unsigned char x = i / fontSize;
		unsigned char y = i % fontSize;
#ifdef MULTIPASS_ARCH_arduino_nano
		data[(height/8) * (fontX + x) + fontY/8 + y] = pgm_read_byte(&glyph_addr[i+1]);
#else
		data[(height/8) * (fontX + x) + fontY/8 + y] = glyph[i+1];
#endif
	}
	for (unsigned char i = 0; i < fontSize; i++) {
		data[(height/8) * (fontX + glyph_w + 1) + fontY/8] = 0;
	}
	fontX += (glyph_w / fontSize) + 2;
}

Framebuffer fb(framebuffer);
