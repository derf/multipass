#include "lib/xdr.h"


void XDRWriter::put(uint32_t number)
{
	*buffer++ = ((number >> 24) & 0xffU);
	*buffer++ = ((number >> 16) & 0xffU);
	*buffer++ = ((number >> 8) & 0xffU);
	*buffer++ = (number & 0xffU);
	pos += 4;
}

void XDRWriter::put(int32_t number)
{
	*buffer++ = ((number >> 24) & 0xffU);
	*buffer++ = ((number >> 16) & 0xffU);
	*buffer++ = ((number >> 8) & 0xffU);
	*buffer++ = (number & 0xffU);
	pos += 4;
}

void XDRWriter::put(uint64_t number)
{
	*buffer++ = ((number >> 56) & 0xffU);
	*buffer++ = ((number >> 48) & 0xffU);
	*buffer++ = ((number >> 40) & 0xffU);
	*buffer++ = ((number >> 32) & 0xffU);
	*buffer++ = ((number >> 24) & 0xffU);
	*buffer++ = ((number >> 16) & 0xffU);
	*buffer++ = ((number >> 8) & 0xffU);
	*buffer++ = (number & 0xffU);
	pos += 8;
}

void XDRWriter::put(int64_t number)
{
	*buffer++ = ((number >> 56) & 0xffU);
	*buffer++ = ((number >> 48) & 0xffU);
	*buffer++ = ((number >> 40) & 0xffU);
	*buffer++ = ((number >> 32) & 0xffU);
	*buffer++ = ((number >> 24) & 0xffU);
	*buffer++ = ((number >> 16) & 0xffU);
	*buffer++ = ((number >> 8) & 0xffU);
	*buffer++ = (number & 0xffU);
	pos += 8;
}

void XDRWriter::put(float number)
{
	union {
		uint32_t i;
		float f;
	} v;
	// Setting one member of a struct and then reading another is undefined
	// behaviour, but works as intended in nearly any (embedded) compiler
	v.f = number;
	put(v.i);
}

void XDRWriter::put(double number)
{
	union {
		uint64_t i;
		double d;
	} v;
	// Setting one member of a struct and then reading another is undefined
	// behaviour, but works as intended in nearly any (embedded) compiler
	v.d = number;
	put(v.i);
}

void XDRWriter::put(char const *data){
	if (!is_fixed_length) {
		put(next_array_len);
	}
	uint32_t i;
	for (i = 0; i < next_array_len; i++) {
		*buffer++ = data[i];
	}
	while ((i++) % 4 != 0){
		*buffer++ = 0;
	}
	pos += i;
}

template<uint32_t TSize>
void XDRWriter::put(char const (&data)[TSize]){
	if (!is_fixed_length) {
		put(TSize);
	}
	uint32_t i;
	for (i = 0; i < TSize; i++) {
		*buffer++ = data[i];
	}
	while ((i++) % 4 != 0){
		*buffer++ = 0;
	}
	pos += i;
}

uint32_t XDRReader::get_uint32()
{
	uint32_t ret = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	return ret;
}

int32_t XDRReader::get_int32()
{
	int32_t ret = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	return ret;
}

uint64_t XDRReader::get_uint64()
{
	uint64_t ret0 = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	uint64_t ret1 = ((uint8_t)data[pos]<<24) | ((uint8_t)data[pos+1]<<16)
		| ((uint8_t)data[pos+2]<<8) | (uint8_t)data[pos+3];
	pos += 4;
	return (ret0 << 32) | ret1;
}

int64_t XDRReader::get_int64()
{
	int64_t ret0 = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	int64_t ret1 = (data[pos]<<24) | (data[pos+1]<<16) | (data[pos+2]<<8) | data[pos+3];
	pos += 4;
	return (ret0 << 32) | ret1;
}

float XDRReader::get_float()
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

double XDRReader::get_double()
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

uint32_t XDRReader::get_opaque_length()
{
	return get_uint32();
}

char *XDRReader::get_opaque(uint32_t length)
{
	char *ret = data + pos;
	pos += length;
	if (length % 4) {
		pos += 4 - (length % 4);
	}
	return ret;
}

void XDRReader::get_string(char* target)
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
