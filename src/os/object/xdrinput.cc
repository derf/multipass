#include "object/xdrinput.h"

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
	// Setting one member of a struct and then reading another is undefined
	// behaviour, but works as intended in nearly any (embedded) compiler
	v.i = get_uint32();
	return v.f;
}

double XDRInput::get_double()
{
	union {
		uint64_t i;
		double d;
	} v;
	// Setting one member of a struct and then reading another is undefined
	// behaviour, but works as intended in nearly any (embedded) compiler
	v.i = get_uint64();
	return v.d;
}

uint32_t XDRInput::get_opaque_length()
{
	return get_uint32();
}

char *XDRInput::get_opaque(uint32_t length)
{
	char *ret = data + pos;
	pos += length;
	if (length % 4) {
		pos += 4 - (length % 4);
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
	if (length % 4) {
		pos += 4 - (length % 4);
	}
}
