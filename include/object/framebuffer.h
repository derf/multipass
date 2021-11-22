#pragma once
#include "object/outputstream.h"

typedef const unsigned char* glyph_t;

class Framebuffer : public OutputStream
{
	private:
		Framebuffer(Framebuffer& copy);
		const glyph_t *font;
		unsigned int fontX, fontY;
		unsigned char scale;

	public:
		unsigned char *data;

		Framebuffer(unsigned char *data) : font(0), fontX(0), fontY(0), scale(1), data(data) {}

		constexpr static unsigned int const width = CONFIG_framebuffer_width;
		constexpr static unsigned int const height = CONFIG_framebuffer_height;

		void clear();
		void scroll();
		void fillBox(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		void drawAt(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *image);
		void setFont(const glyph_t *font) { this->font = font; }
		void setPos(unsigned int newX, unsigned int newY) { fontX = newX; fontY = newY; }
		virtual void put(char c) override;
};

extern Framebuffer fb;
