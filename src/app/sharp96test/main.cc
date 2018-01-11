#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/spi_b.h"
#include "driver/sharp96.h"

unsigned char const lynx[12 * 96] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x03, 0x0C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x0C, 0x16, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x18, 0x0A, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x10, 0x0A, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x30, 0x0B, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x60, 0x0D, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x40, 0x2D, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x80, 0x3D, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0x81, 0x80, 0x1D, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0xC1, 0x00, 0x1C, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0xE3, 0x00, 0xFC, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0xF6, 0x11, 0xFC, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0xFE, 0x10, 0x7C, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1F, 0xFE, 0x10, 0x7C, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3F, 0xFE, 0x10, 0xFC, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F, 0x10, 0xFF, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0xFF, 0x0F, 0x19, 0xFF, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0xFF, 0x80, 0x0F, 0xFF, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x71, 0xC0, 0xFF, 0x7F, 0xE0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x38, 0x70, 0x40, 0x7C, 0x7F, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x70, 0x60, 0x20, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xC0, 0xE0, 0x00, 0x00, 0xE1, 0xB8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x07, 0x81, 0xC0, 0x00, 0x00, 0x70, 0x9E, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x81, 0xC0, 0x00, 0x00, 0x70, 0x1F, 0xC0, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0x0F, 0xE0, 0x00, 0x00,
	0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x0F, 0xF8, 0x00, 0x00,
	0x00, 0x00, 0x0C, 0x01, 0xFE, 0x00, 0x3F, 0x80, 0x0F, 0xFE, 0x00, 0x00,
	0x00, 0x00, 0x0C, 0x03, 0xE0, 0x00, 0x0F, 0x80, 0x0F, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x0C, 0x07, 0x18, 0x00, 0x03, 0x8C, 0x07, 0xFF, 0x80, 0x00,
	0x00, 0x00, 0x08, 0x3E, 0x60, 0x00, 0x01, 0xCC, 0x03, 0xFF, 0xC0, 0x00,
	0x00, 0x00, 0x08, 0x39, 0xC0, 0x00, 0x00, 0xFC, 0x03, 0x80, 0xE0, 0x00,
	0x00, 0x00, 0x18, 0x7F, 0xE0, 0x01, 0xE0, 0x7E, 0x00, 0x00, 0x10, 0x00,
	0x00, 0x00, 0x33, 0xC4, 0x00, 0x00, 0x3C, 0x3F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x07, 0x9F, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xEC, 0x00, 0x01, 0xFE, 0x01, 0xEF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x06, 0x10, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00,
	0x00, 0x0E, 0x20, 0x1C, 0x00, 0x00, 0x0F, 0x07, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0xA0, 0x70, 0x00, 0x00, 0x01, 0xC7, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0xC1, 0xC6, 0x00, 0x0F, 0xFF, 0xF3, 0xE0, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0xC3, 0x1C, 0x00, 0x00, 0x1F, 0xFD, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x07, 0x86, 0x3F, 0x80, 0x00, 0x03, 0xFF, 0xF8, 0x00, 0x00, 0x00,
	0x00, 0x07, 0x99, 0xFF, 0xE0, 0x00, 0x07, 0xFF, 0xFC, 0x00, 0x00, 0x00,
	0x00, 0x03, 0xFF, 0xFD, 0xF8, 0x00, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00,
	0x00, 0x01, 0xFF, 0xC5, 0xFE, 0x00, 0x06, 0x3F, 0xFC, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xE2, 0x6F, 0xFF, 0x80, 0x03, 0x8F, 0xF6, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x02, 0x5F, 0xFF, 0xC0, 0x00, 0xC7, 0xF0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x21, 0x43, 0xFE, 0xF0, 0x00, 0x73, 0xF8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0xC0, 0xFE, 0xF8, 0x00, 0x11, 0xF8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1C, 0xC0, 0x7E, 0xF8, 0x00, 0x08, 0xF8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0C, 0x40, 0x36, 0x7C, 0x0C, 0x04, 0x7C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x17, 0x7E, 0x07, 0x80, 0x3C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0B, 0x3E, 0x01, 0xE0, 0x1E, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0B, 0x9E, 0x00, 0xF0, 0x0E, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xCF, 0x00, 0x7C, 0x07, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xEF, 0x00, 0x3E, 0x03, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0xFF, 0x00, 0x1F, 0x00, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x7F, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x33, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xDE, 0x01, 0x3B, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0x01, 0xBF, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x01, 0xFF, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x11, 0x0E, 0x03, 0xFF, 0xC0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x6E, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x07, 0xFF, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x07, 0xFF, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE3, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x82, 0xEE, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void loop(void)
{
	static unsigned char i = 0;
	gpio.led_toggle(1);
	sharp96.writeLine(i, (unsigned char *)lynx + (12 * i));
	sharp96.toggleVCOM();
	if (++i == 96) {
		i = 0;
		sharp96.clear();
	}
}

int main(void)
{
	unsigned char i = 0;
	arch.setup();
	gpio.setup();
	kout.setup();
	spi.setup();
	sharp96.setup();
	sharp96.powerOn();
	sharp96.clear();
	for (i = 0; i < 96; i++) {
		sharp96.writeLine(i, (unsigned char *)lynx + (12 * i));
	}

	arch.idle_loop();

	return 0;
}
