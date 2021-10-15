#pragma once

class Framebuffer
{
	public:
		unsigned char *data;

		Framebuffer(unsigned char *data) : data(data) {}

		constexpr static unsigned int const width = CONFIG_framebuffer_width;
		constexpr static unsigned int const height = CONFIG_framebuffer_height;

		void clear();
		void fillBox(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		void drawAt(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *image);

	private:
		Framebuffer(Framebuffer& copy);
};

extern Framebuffer fb;
