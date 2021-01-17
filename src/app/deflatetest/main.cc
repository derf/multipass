/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

unsigned char const deflate_input[] = {
	120, 1, 5, 193, 193, 13, 192, 32, 16, 3, 193, 86, 182, 182, 196, 68, 220,
	135, 147, 12, 86, 218, 103, 102, 198, 70, 133, 98, 147, 37, 118, 243, 143,
	58, 195, 100, 137, 221, 124, 237, 195, 140, 141, 10, 197, 102, 191, 51, 79,
	41, 23, 153, 255, 22, 11
};

unsigned char* udeflate_input_end;
unsigned char* udeflate_input_now;
uint8_t udeflate_bit_offset = 0;

unsigned char deflate_output[1024];

uint16_t const udeflate_length_offsets[] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67,
	83, 99, 115, 131, 163, 195, 227, 258
};

// TODO (index < 4 || index == 28) ? 0 : (index-4) >> 2
uint8_t const udeflate_length_bits[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
	5, 5, 5, 0
};

uint16_t const udeflate_distance_offsets[] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
	769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};

// TODO index < 2 ? 0 : (index-2) >> 1
uint8_t const udeflate_distance_bits[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
	11, 11, 12, 12, 13, 13
};

uint8_t const udeflate_hclen_index[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

uint8_t udeflate_hc_lengths[19];
uint8_t udeflate_lld_lengths[337];

static uint16_t udeflate_rev_word(uint16_t word, uint8_t bits)
{
	uint16_t ret = 0;
	uint16_t mask = 1;
	for (uint16_t rmask = 1 << (bits-1); rmask > 0; rmask >>= 1) {
		if (word & rmask) {
			ret |= mask;
		}
		mask <<= 1;
	}
	return ret;
}

static uint8_t udeflate_bitmask(uint8_t bit_count)
{
	return (1 << bit_count) - 1;
}

static uint16_t udeflate_get_word()
{
	uint16_t ret = 0;
	ret |= (udeflate_input_now[0] >> udeflate_bit_offset);
	ret |= (uint16_t)udeflate_input_now[1] << (8 - udeflate_bit_offset);
	if (udeflate_bit_offset) {
		ret |= (uint16_t)(udeflate_input_now[2] & udeflate_bitmask(udeflate_bit_offset)) << (16 - udeflate_bit_offset);
	}
	kout << "get_word = " << bin << ret << dec << endl;
	return ret;
}

static uint16_t udeflate_get_bits(uint8_t num_bits)
{
	uint16_t ret = udeflate_get_word();
	udeflate_bit_offset += num_bits;
	while (udeflate_bit_offset >= 8) {
		udeflate_input_now++;
		udeflate_bit_offset -= 8;
	}
	return ret & udeflate_bitmask(num_bits);
}

/*
 * Assumptions:
 * * huffman code length is limited to 11 bits
 * * there are no more than 255 huffman codes with the same length
 *
 * Rationale: longer huffman codes might appear when handling large data
 * sets. We don't do that; instead, we expect the uncompressed source to
 * be no more than a few kB of data.
 */
uint8_t udeflate_bl_count[12];
uint16_t udeflate_next_code[12];

// TODO specify bl_count and next_code lists
// (length and distance codes may use separate alphabets)
static void udeflate_build_alphabet(uint8_t* lengths, uint16_t size)
{
	uint16_t i;
	uint16_t code = 0;
	uint8_t max_len = 0;
	for (i = 0; i < 12; i++) {
		udeflate_bl_count[i] = 0;
	}

	for (i = 0; i < size; i++) {
		if (lengths[i]) {
			udeflate_bl_count[lengths[i]]++;
		}
		if (lengths[i] > max_len) {
			max_len = lengths[i];
		}
	}

	for (i = 1; i < max_len+1; i++) {
		code = (code + udeflate_bl_count[i-1]) << 1;
		udeflate_next_code[i] = code;
	}

	for (i = 0; i < 12; i++) {
		kout << "bl_count[" << i << "] = " << udeflate_bl_count[i] << endl;
	}
	for (i = 0; i < 12; i++) {
		kout << "next_code[" << i << "] = " << udeflate_next_code[i] << endl;
	}
}

static uint16_t udeflate_huff(uint8_t* lengths, uint16_t size)
{
	uint16_t next_word = udeflate_get_word();
	for (uint8_t num_bits = 1; num_bits < 12; num_bits++) {
		uint16_t next_bits = udeflate_rev_word(next_word, num_bits);
		// TODO benötigt bit reversal bei der Abgleichung code <-> next_bits
		// (geeignete is_bit_eq(next_bits, code_candidate, num_bits) Funktion?)
		if (udeflate_bl_count[num_bits] && next_bits >= udeflate_next_code[num_bits] && next_bits < udeflate_next_code[num_bits] + udeflate_bl_count[num_bits] ) {
			kout << "found huffman code, length = " << num_bits << endl;
			udeflate_bit_offset += num_bits;
			while (udeflate_bit_offset >= 8) {
				udeflate_input_now++;
				udeflate_bit_offset -= 8;
			}
			uint8_t len_pos = next_bits;
			uint8_t cur_pos = udeflate_next_code[num_bits];
			for (uint16_t i = 0; i < size; i++) {
				if (lengths[i] == num_bits) {
					if (cur_pos == len_pos) {
						return i;
					}
					cur_pos++;
				}
			}
		}
	}
	return 65535;
}

static int8_t udeflate_dynamic_huffman()
{
	uint8_t i;
	uint16_t hlit = 257 + udeflate_get_bits(5);
	kout << "hlit=" << hlit << endl;
	uint8_t hdist = 1 + udeflate_get_bits(5);
	kout << "hdist=" << hdist << endl;
	uint8_t hclen = 4 + udeflate_get_bits(4);
	kout << "hclen=" << hclen << endl;

	for (i = 0; i < hclen; i++) {
		udeflate_hc_lengths[udeflate_hclen_index[i]] = udeflate_get_bits(3);
	}
	for (i = hclen; i < sizeof(udeflate_hc_lengths); i++) {
		udeflate_hc_lengths[udeflate_hclen_index[i]] = 0;
	}

	udeflate_build_alphabet(udeflate_hc_lengths, sizeof(udeflate_hc_lengths));

	uint16_t items_processed = 0;
	while (items_processed < hlit + hdist) {
		uint8_t code = udeflate_huff(udeflate_hc_lengths, 19);
		kout << "code = " << code << endl;
		if (code == 16) {
			uint8_t copy_count = 3 + udeflate_get_bits(2);
			for (uint8_t i = 0; i < copy_count; i++) {
				udeflate_lld_lengths[items_processed] = udeflate_lld_lengths[items_processed-1];
				items_processed++;
			}
		}
		else if (code == 17) {
			uint8_t null_count = 3 + udeflate_get_bits(3);
			for (uint8_t i = 0; i < null_count; i++) {
				udeflate_lld_lengths[items_processed] = 0;
				items_processed++;
			}
		}
		else if (code == 18) {
			uint8_t null_count = 11 + udeflate_get_bits(7);
			for (uint8_t i = 0; i < null_count; i++) {
				udeflate_lld_lengths[items_processed] = 0;
				items_processed++;
			}
		}
		else {
			udeflate_lld_lengths[items_processed] = code;
			items_processed++;
		}
	}

	return 0;
}

int8_t udeflate(unsigned char* buf, uint16_t buf_len)
{
	uint8_t is_final = buf[0] & 0x01;
	uint8_t block_type = (buf[0] & 0x06) >> 1;
	kout << "is_final=" << is_final << " block_type=" << block_type << endl;

	udeflate_input_now = buf;
	udeflate_input_end = buf + buf_len;
	udeflate_bit_offset = 3;

	if (block_type == 2) {
		return udeflate_dynamic_huffman();
	}

	return -4;
}

int8_t udeflate_zlib(unsigned char* buf, uint16_t buf_len)
{
	if (buf_len < 4) {
		return -1;
	}
	uint8_t zlib_method = buf[0] & 0x0f;
	uint16_t zlib_window_size = 1 << (8 + ((buf[0] & 0xf0) >> 4));
	uint8_t zlib_flags = buf[1];
	kout << "zlib_method=" << zlib_method << endl;
	kout << "zlib_window_size=" << zlib_window_size << endl;

	if (zlib_method != 8) {
		return -2;
	}

	if (zlib_flags & 0x10) {
		return -3;
	}

	return udeflate(buf+2, buf_len-2);
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	for (uint8_t i = 0; i < 5; i++) {
		int8_t ret = udeflate_zlib((unsigned char*)deflate_input, sizeof(deflate_input));
		kout << "udeflate returned " << ret << endl;
	}

	arch.idle();

	return 0;
}
