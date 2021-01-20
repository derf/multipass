/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>

#define UDEFLATE_ERR_LENGTH (-1)
#define UDEFLATE_ERR_METHOD (-2)
#define UDEFLATE_ERR_FDICT (-3)
#define UDEFLATE_ERR_BLOCK (-4)
#define UDEFLATE_ERR_CHECKSUM (-5)

int8_t udeflate(unsigned char *input_buf, uint16_t input_len,
		unsigned char *output_buf, uint16_t output_len);
int8_t udeflate_zlib(unsigned char *input_buf, uint16_t input_len,
		     unsigned char *output_buf, uint16_t output_len);
