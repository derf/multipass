#pragma once
#include "object/outputstream.h"

typedef const unsigned char* glyph_t;

class Framebuffer : public OutputStream
{
	private:
		Framebuffer(Framebuffer& copy);
		const glyph_t *font;
		unsigned int fontX, fontY;
		unsigned char fontSize;
		unsigned char scale;

	public:
		unsigned char *data;

		Framebuffer(unsigned char *data) : font(0), fontX(0), fontY(0), fontSize(1), scale(1), data(data) {}

		constexpr static unsigned int const width = CONFIG_framebuffer_width;
		constexpr static unsigned int const height = CONFIG_framebuffer_height;

		void clear();
		void scroll();
		void fillBox(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		void drawAt(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *image);
		void drawBattery(unsigned int x, unsigned int y, unsigned char percent, bool charging = false);
		void setFont(const glyph_t *font, unsigned char fontSize = 1) { this->font = font; this->fontSize = fontSize; }
		void setPos(unsigned int newX, unsigned int newY) { fontX = newX; fontY = newY; }
		virtual void put(char c) override;
};

extern Framebuffer fb;
