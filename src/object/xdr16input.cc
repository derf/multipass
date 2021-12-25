/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "object/xdr16input.h"

uint16_t XDRInput::get_uint16()
{
	uint32_t ret = ((uint8_t)data[pos]<<8) | (uint8_t)data[pos+1];
	pos += 2;
	return ret;
}

int16_t XDRInput::get_int16()
{
	int32_t ret = (data[pos]<<8) | data[pos+1];
	pos += 2;
	return ret;
}

uint32_t XDRInput::get_uint32()
{
	uint32_t ret = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	return ret;
}

int32_t XDRInput::get_int32()
{
	int32_t ret = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	return ret;
}

uint64_t XDRInput::get_uint64()
{
	uint64_t ret0 = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	uint64_t ret1 = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	return (ret0 << 32) | ret1;
}

int64_t XDRInput::get_int64()
{
	int64_t ret0 = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	int64_t ret1 = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	return (ret0 << 32) | ret1;
}

float XDRInput::get_float()
{
	union {
		uint32_t i;
		float f;
	} v;
	v.i = get_uint32();
	return v.f;
}

double XDRInput::get_double()
{
	union {
		uint64_t i;
		double d;
	} v;
	v.i = get_uint64();
	return v.d;
}

uint16_t XDRInput::get_opaque_length()
{
	return get_uint16();
}

char *XDRInput::get_opaque(uint16_t length)
{
	char *ret = data + pos;
	pos += length;
	if (length % 2) {
		pos += 2 - (length % 2);
	}
	return ret;
}

void XDRInput::get_string(char* target)
{
	uint16_t length = get_opaque_length();
	uint16_t i;
	for (i = 0; i < length; i++) {
		target[i] = data[pos + i];
	}
	target[i] = 0;
	pos += length;
	if (length % 2) {
		pos += 2 - (length % 2);
	}
}
