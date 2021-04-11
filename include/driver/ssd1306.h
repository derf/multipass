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

		const unsigned char init1[6] = {
			SSD1306_SET_DISP | 0x00,
			SSD1306_SET_MEM_ADDR, 0x00,
			SSD1306_SET_DISP_START_LINE | 0x00,
			SSD1306_SET_SEG_REMAP | 0x01,
			SSD1306_SET_MUX_RATIO
		};
		// height
		const unsigned char init2[4] = {
			SSD1306_SET_COM_OUT_DIR | 0x08,
			SSD1306_SET_DISP_OFFSET, 0x00,
			SSD1306_SET_COM_PIN_CFG
		};
		// height == 32? 0x02 : 0x12
		const unsigned char init3[13] = {
			SSD1306_SET_DISP_CLK_DIV, 0x80,
			SSD1306_SET_PRECHARGE, 0xf1,
			SSD1306_SET_VCOM_DESEL, 0x30,
			SSD1306_SET_CONTRAST, 0x7f,
			SSD1306_SET_ENTIRE_ON,
			SSD1306_SET_NORM_INV,
			SSD1306_SET_CHARGE_PUMP, 0x14,
			SSD1306_SET_DISP | 0x01
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
