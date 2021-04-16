/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef SSD1306_H
#define SSD1306_H

#define SSD1306_SET_CONTRAST 0x81
#define SSD1306_SET_ENTIRE_ON 0xa4
#define SSD1306_SET_NORM_INV 0xa6
#define SSD1306_SET_DISP 0xae
#define SSD1306_SET_MEM_ADDR 0x20
#define SSD1306_SET_COL_ADDR 0x21
#define SSD1306_SET_PAGE_ADDR 0x22
#define SSD1306_SET_DISP_START_LINE 0x40
#define SSD1306_SET_SEG_REMAP 0xa0
#define SSD1306_SET_MUX_RATIO 0xa8
#define SSD1306_SET_COM_OUT_DIR 0xc0
#define SSD1306_SET_DISP_OFFSET 0xd3
#define SSD1306_SET_COM_PIN_CFG 0xda
#define SSD1306_SET_DISP_CLK_DIV 0xd5
#define SSD1306_SET_PRECHARGE 0xd9
#define SSD1306_SET_VCOM_DESEL 0xdb
#define SSD1306_SET_CHARGE_PUMP 0x8d

#include <stdint.h>
#include <stddef.h>

class SSD1306 {
	private:
		const uint8_t width = SSD1306_WIDTH;
		const uint8_t height = SSD1306_HEIGHT;
		SSD1306(const SSD1306 &copy);

		unsigned char const address = 0x3c;

		unsigned char txbuf[130];
		unsigned char rxbuf[2];

		/*
		 * Adjust both SSD1306_SET_SEG_REMAP and SSD1306_SET_COM_OUT_DIR for
		 * 180° rotation.
		 */
		const unsigned char init1[6] = {
			// Turn off power for configuration
			SSD1306_SET_DISP | 0x00,

			/*
			 * Enable Horizontal Addressing Mode. Assuming image data is
			 * {A, B, C, ...}, each byte corresponds to a 1x8 column, starting
			 * at the top left corner and proceeding to the right and then down:
			 *
			 * A7 B7 C7 ...
			 * .. .. .. ...
			 * A0 B0 C0 ...
			 */
			SSD1306_SET_MEM_ADDR, 0x00,

			// RAM line 0 == display line 0
			SSD1306_SET_DISP_START_LINE | 0x00,

			/*
			 * Horizontal Layout: Column 127 is SEG0.
			 * This depends on the connection between SSD1306 and OLED.
			 * Use 0x00 if your content is horizontally mirrored.
			 */
			SSD1306_SET_SEG_REMAP | 0x01,

			/*
			 * Multiplex ratio is the number of display lines
			 * (i.e., the display height)
			 */
			SSD1306_SET_MUX_RATIO
		};

		// height-1 sent by init()

		const unsigned char init2[4] = {
			/*
			 * Vertical Layout: Scan from COM63 to COM0.
			 * This depends on the connection between SSD1306 and OLED.
			 * Use 0x00 if your content is vertically mirrored.
			 */
			SSD1306_SET_COM_OUT_DIR | 0x08,

			// No vertical display offset
			SSD1306_SET_DISP_OFFSET, 0x00,

			/*
			 * COM PIN layout depends on display height and type. See
			 * datasheet and init().
			 */
			SSD1306_SET_COM_PIN_CFG
		};

		// height == 32? 0x02 : 0x12

		const unsigned char init3[19] = {
			/*
			 * Set clock to recommended values: 370 kHz (bits 7..4),
			 * no divider (bits 3..0). Increase divider for glitchy effects.
			 * The datasheet is unclear on the clock frequency range.
			 */
			SSD1306_SET_DISP_CLK_DIV, 0x80,

			/*
			 * Set line (multiplex) precharge times.
			 * phase 1 (discharge pixels to avoid effects from the previous line): 8 cycles
			 * phase 2 (charge pixels for next line): 8 cycles
			 * decrease phase 1 time for glitchy effects.
			 */
			SSD1306_SET_PRECHARGE, 0x88,

			/*
			 * VCOM deselect level. Unknown.
			 * 0x00: ~0.65 VCC
			 * 0x20: ~0.77 VCC
			 * 0x30: ~0.85 VCC
			 */
			SSD1306_SET_VCOM_DESEL, 0x30,

			// start with medium contrast
			SSD1306_SET_CONTRAST, 0x7f,

			// display content == RAM content
			SSD1306_SET_ENTIRE_ON,

			// regular (uninverted) display
			SSD1306_SET_NORM_INV | 0x00,

			// Enable charge pump (provide power to the display)
			SSD1306_SET_CHARGE_PUMP, 0x14,

			// turn on display
			SSD1306_SET_DISP | 0x01,

			// reset column pointer
			SSD1306_SET_COL_ADDR, 0, 127,

			// reset page pointer
			SSD1306_SET_PAGE_ADDR, 0, 7
		};

		void writeCommand(uint8_t command);
		void writeData(unsigned char* data);

	public:
		SSD1306() {}

		void init();
		void setContrast(unsigned char contrast);
		void setInvert(bool invert);
		void showImage(unsigned char* data, uint16_t length);
};

extern SSD1306 ssd1306;

#endif
