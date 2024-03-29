#pragma once
/*
 * Pixel Operator Font
 * <https://notabug.org/HarvettFox96/ttf-pixeloperator>
 * <https://www.dafont.com/pixel-operator.font>
 *
 * Copyright 2018 Jayvee Enaguas
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#ifdef MULTIPASS_ARCH_arduino_nano
#include <avr/pgmspace.h>
#define GLYPH_ATTR PROGMEM
#else
#define GLYPH_ATTR
#endif

typedef const unsigned char glyph_line_t;
typedef const unsigned char* glyph_t;

glyph_line_t GLYPH_ATTR chr_032[] = {0x03,0x00,0x00,0x00}; // <space>
glyph_line_t GLYPH_ATTR chr_033[] = {0x01,0x7D}; // !
glyph_line_t GLYPH_ATTR chr_034[] = {0x04,0x30,0x40,0x30,0x40}; // "
glyph_line_t GLYPH_ATTR chr_035[] = {0x06,0x12,0x3F,0x12,0x12,0x3F,0x12}; // #
glyph_line_t GLYPH_ATTR chr_036[] = {0x05,0x12,0x2A,0x7F,0x2A,0x24}; // $
glyph_line_t GLYPH_ATTR chr_037[] = {0x07,0x30,0x4A,0x34,0x08,0x16,0x29,0x06}; // %
glyph_line_t GLYPH_ATTR chr_038[] = {0x05,0x36,0x49,0x49,0x49,0x27}; // &
glyph_line_t GLYPH_ATTR chr_039[] = {0x01,0x70}; // '
glyph_line_t GLYPH_ATTR chr_040[] = {0x03,0x1C,0x22,0x41}; // (
glyph_line_t GLYPH_ATTR chr_041[] = {0x03,0x41,0x22,0x1C}; // )
glyph_line_t GLYPH_ATTR chr_042[] = {0x05,0x28,0x10,0x7C,0x10,0x28}; // *
glyph_line_t GLYPH_ATTR chr_043[] = {0x05,0x08,0x08,0x3E,0x08,0x08}; // +
glyph_line_t GLYPH_ATTR chr_044[] = {0x02,0x01,0x02}; // ,
glyph_line_t GLYPH_ATTR chr_045[] = {0x05,0x08,0x08,0x08,0x08,0x08}; // -
glyph_line_t GLYPH_ATTR chr_046[] = {0x01,0x01}; // .
glyph_line_t GLYPH_ATTR chr_047[] = {0x03,0x03,0x1C,0x60}; // /
glyph_line_t GLYPH_ATTR chr_048[] = {0x05,0x3E,0x45,0x49,0x51,0x3E}; // 0
glyph_line_t GLYPH_ATTR chr_049[] = {0x03,0x10,0x20,0x7F}; // 1
glyph_line_t GLYPH_ATTR chr_050[] = {0x05,0x21,0x43,0x45,0x49,0x31}; // 2
glyph_line_t GLYPH_ATTR chr_051[] = {0x05,0x22,0x41,0x49,0x49,0x36}; // 3
glyph_line_t GLYPH_ATTR chr_052[] = {0x05,0x0C,0x14,0x24,0x44,0x7F}; // 4
glyph_line_t GLYPH_ATTR chr_053[] = {0x05,0x72,0x51,0x51,0x51,0x4E}; // 5
glyph_line_t GLYPH_ATTR chr_054[] = {0x05,0x3E,0x49,0x49,0x49,0x26}; // 6
glyph_line_t GLYPH_ATTR chr_055[] = {0x05,0x43,0x44,0x48,0x50,0x60}; // 7
glyph_line_t GLYPH_ATTR chr_056[] = {0x05,0x36,0x49,0x49,0x49,0x36}; // 8
glyph_line_t GLYPH_ATTR chr_057[] = {0x05,0x32,0x49,0x49,0x49,0x3E}; // 9
glyph_line_t GLYPH_ATTR chr_058[] = {0x01,0x12}; // :
glyph_line_t GLYPH_ATTR chr_059[] = {0x02,0x01,0x12}; // ;
glyph_line_t GLYPH_ATTR chr_060[] = {0x03,0x08,0x14,0x22}; // <
glyph_line_t GLYPH_ATTR chr_061[] = {0x05,0x14,0x14,0x14,0x14,0x14}; // =
glyph_line_t GLYPH_ATTR chr_062[] = {0x03,0x22,0x14,0x08}; // >
glyph_line_t GLYPH_ATTR chr_063[] = {0x05,0x20,0x40,0x45,0x48,0x30}; // ?
glyph_line_t GLYPH_ATTR chr_064[] = {0x07,0x3E,0x41,0x49,0x55,0x5D,0x45,0x38}; // @
glyph_line_t GLYPH_ATTR chr_065[] = {0x05,0x3F,0x48,0x48,0x48,0x3F}; // A
glyph_line_t GLYPH_ATTR chr_066[] = {0x05,0x7F,0x49,0x49,0x49,0x36}; // B
glyph_line_t GLYPH_ATTR chr_067[] = {0x05,0x3E,0x41,0x41,0x41,0x22}; // C
glyph_line_t GLYPH_ATTR chr_068[] = {0x05,0x7F,0x41,0x41,0x41,0x3E}; // D
glyph_line_t GLYPH_ATTR chr_069[] = {0x05,0x7F,0x49,0x49,0x41,0x41}; // E
glyph_line_t GLYPH_ATTR chr_070[] = {0x05,0x7F,0x48,0x48,0x40,0x40}; // F
glyph_line_t GLYPH_ATTR chr_071[] = {0x05,0x3E,0x41,0x41,0x49,0x2F}; // G
glyph_line_t GLYPH_ATTR chr_072[] = {0x05,0x7F,0x08,0x08,0x08,0x7F}; // H
glyph_line_t GLYPH_ATTR chr_073[] = {0x01,0x7F}; // I
glyph_line_t GLYPH_ATTR chr_074[] = {0x05,0x02,0x01,0x01,0x01,0x7E}; // J
glyph_line_t GLYPH_ATTR chr_075[] = {0x05,0x7F,0x08,0x14,0x22,0x41}; // K
glyph_line_t GLYPH_ATTR chr_076[] = {0x05,0x7F,0x01,0x01,0x01,0x01}; // L
glyph_line_t GLYPH_ATTR chr_077[] = {0x07,0x7F,0x10,0x08,0x04,0x08,0x10,0x7F}; // M
glyph_line_t GLYPH_ATTR chr_078[] = {0x05,0x7F,0x10,0x08,0x04,0x7F}; // N
glyph_line_t GLYPH_ATTR chr_079[] = {0x05,0x3E,0x41,0x41,0x41,0x3E}; // O
glyph_line_t GLYPH_ATTR chr_080[] = {0x05,0x7F,0x48,0x48,0x48,0x30}; // P
glyph_line_t GLYPH_ATTR chr_081[] = {0x05,0x3E,0x41,0x45,0x42,0x3D}; // Q
glyph_line_t GLYPH_ATTR chr_082[] = {0x05,0x7F,0x44,0x44,0x46,0x39}; // R
glyph_line_t GLYPH_ATTR chr_083[] = {0x05,0x32,0x49,0x49,0x49,0x26}; // S
glyph_line_t GLYPH_ATTR chr_084[] = {0x05,0x40,0x40,0x7F,0x40,0x40}; // T
glyph_line_t GLYPH_ATTR chr_085[] = {0x05,0x7E,0x01,0x01,0x01,0x7E}; // U
glyph_line_t GLYPH_ATTR chr_086[] = {0x05,0x7C,0x02,0x01,0x02,0x7C}; // V
glyph_line_t GLYPH_ATTR chr_087[] = {0x07,0x7E,0x01,0x01,0x1E,0x01,0x01,0x7E}; // W
glyph_line_t GLYPH_ATTR chr_088[] = {0x05,0x63,0x14,0x08,0x14,0x63}; // X
glyph_line_t GLYPH_ATTR chr_089[] = {0x05,0x60,0x10,0x0F,0x10,0x60}; // Y
glyph_line_t GLYPH_ATTR chr_090[] = {0x05,0x43,0x45,0x49,0x51,0x61}; // Z
glyph_line_t GLYPH_ATTR chr_091[] = {0x03,0x7F,0x41,0x41}; // [
glyph_line_t GLYPH_ATTR chr_092[] = {0x03,0x60,0x1C,0x03}; // backslash
glyph_line_t GLYPH_ATTR chr_093[] = {0x03,0x41,0x41,0x7F}; // ]
glyph_line_t GLYPH_ATTR chr_094[] = {0x05,0x10,0x20,0x40,0x20,0x10}; // ^
glyph_line_t GLYPH_ATTR chr_095[] = {0x05,0x01,0x01,0x01,0x01,0x01}; // _
glyph_line_t GLYPH_ATTR chr_096[] = {0x02,0x40,0x20}; // `
glyph_line_t GLYPH_ATTR chr_097[] = {0x05,0x02,0x15,0x15,0x15,0x0F}; // a
glyph_line_t GLYPH_ATTR chr_098[] = {0x05,0x7F,0x11,0x11,0x11,0x0E}; // b
glyph_line_t GLYPH_ATTR chr_099[] = {0x05,0x0E,0x11,0x11,0x11,0x0A}; // c
glyph_line_t GLYPH_ATTR chr_100[] = {0x05,0x0E,0x11,0x11,0x11,0x7F}; // d
glyph_line_t GLYPH_ATTR chr_101[] = {0x05,0x0E,0x15,0x15,0x15,0x0C}; // e
glyph_line_t GLYPH_ATTR chr_102[] = {0x05,0x10,0x3F,0x50,0x50,0x40}; // f
glyph_line_t GLYPH_ATTR chr_103[] = {0x05,0x08,0x15,0x15,0x15,0x1E}; // g
glyph_line_t GLYPH_ATTR chr_104[] = {0x05,0x7F,0x10,0x10,0x10,0x0F}; // h
glyph_line_t GLYPH_ATTR chr_105[] = {0x01,0x5F}; // i
glyph_line_t GLYPH_ATTR chr_106[] = {0x05,0x02,0x01,0x01,0x01,0x5E}; // j
glyph_line_t GLYPH_ATTR chr_107[] = {0x05,0x7F,0x04,0x0C,0x12,0x01}; // k
glyph_line_t GLYPH_ATTR chr_108[] = {0x01,0x7F}; // l
glyph_line_t GLYPH_ATTR chr_109[] = {0x07,0x1F,0x10,0x10,0x0C,0x10,0x10,0x0F}; // m
glyph_line_t GLYPH_ATTR chr_110[] = {0x05,0x1F,0x10,0x10,0x10,0x0F}; // n
glyph_line_t GLYPH_ATTR chr_111[] = {0x05,0x0E,0x11,0x11,0x11,0x0E}; // o
glyph_line_t GLYPH_ATTR chr_112[] = {0x05,0x0F,0x14,0x14,0x14,0x08}; // p
glyph_line_t GLYPH_ATTR chr_113[] = {0x05,0x08,0x14,0x14,0x14,0x0F}; // q
glyph_line_t GLYPH_ATTR chr_114[] = {0x05,0x1F,0x04,0x08,0x10,0x10}; // r
glyph_line_t GLYPH_ATTR chr_115[] = {0x05,0x09,0x15,0x15,0x15,0x02}; // s
glyph_line_t GLYPH_ATTR chr_116[] = {0x05,0x10,0x3E,0x11,0x11,0x01}; // t
glyph_line_t GLYPH_ATTR chr_117[] = {0x05,0x1E,0x01,0x01,0x01,0x1E}; // u
glyph_line_t GLYPH_ATTR chr_118[] = {0x05,0x1C,0x02,0x01,0x02,0x1C}; // v
glyph_line_t GLYPH_ATTR chr_119[] = {0x07,0x1E,0x01,0x01,0x02,0x01,0x01,0x1E}; // w
glyph_line_t GLYPH_ATTR chr_120[] = {0x05,0x11,0x0A,0x04,0x0A,0x11}; // x
glyph_line_t GLYPH_ATTR chr_121[] = {0x05,0x19,0x05,0x05,0x05,0x1E}; // y
glyph_line_t GLYPH_ATTR chr_122[] = {0x05,0x11,0x13,0x15,0x19,0x11}; // z
glyph_line_t GLYPH_ATTR chr_123[] = {0x04,0x08,0x36,0x41,0x41}; // {
glyph_line_t GLYPH_ATTR chr_124[] = {0x01,0x7F}; // |
glyph_line_t GLYPH_ATTR chr_125[] = {0x04,0x41,0x41,0x36,0x08}; // }
glyph_line_t GLYPH_ATTR chr_126[] = {0x06,0x20,0x40,0x40,0x20,0x20,0x40}; // ~

const glyph_t pixeloperator[] GLYPH_ATTR = {
	chr_032,
	chr_033,
	chr_034,
	chr_035,
	chr_036,
	chr_037,
	chr_038,
	chr_039,
	chr_040,
	chr_041,
	chr_042,
	chr_043,
	chr_044,
	chr_045,
	chr_046,
	chr_047,
	chr_048,
	chr_049,
	chr_050,
	chr_051,
	chr_052,
	chr_053,
	chr_054,
	chr_055,
	chr_056,
	chr_057,
	chr_058,
	chr_059,
	chr_060,
	chr_061,
	chr_062,
	chr_063,
	chr_064,
	chr_065,
	chr_066,
	chr_067,
	chr_068,
	chr_069,
	chr_070,
	chr_071,
	chr_072,
	chr_073,
	chr_074,
	chr_075,
	chr_076,
	chr_077,
	chr_078,
	chr_079,
	chr_080,
	chr_081,
	chr_082,
	chr_083,
	chr_084,
	chr_085,
	chr_086,
	chr_087,
	chr_088,
	chr_089,
	chr_090,
	chr_091,
	chr_092,
	chr_093,
	chr_094,
	chr_095,
	chr_096,
	chr_097,
	chr_098,
	chr_099,
	chr_100,
	chr_101,
	chr_102,
	chr_103,
	chr_104,
	chr_105,
	chr_106,
	chr_107,
	chr_108,
	chr_109,
	chr_110,
	chr_111,
	chr_112,
	chr_113,
	chr_114,
	chr_115,
	chr_116,
	chr_117,
	chr_118,
	chr_119,
	chr_120,
	chr_121,
	chr_122,
	chr_123,
	chr_124,
	chr_125,
	chr_126
};
