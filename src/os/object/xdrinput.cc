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