#ifndef XDRINPUT_H
#define XDRINPUT_H

#include <stdint.h>

class XDRInput {
	private:
		XDRInput(const XDRInput& copy);
		char *data;
		uint32_t pos;

	public:
		XDRInput(char *d) : pos(0) { data = d; }
		uint32_t get_uint32();
		int32_t get_int32();
		uint64_t get_uint64();
		int64_t get_int64();
		float get_float();
		double get_double();
		uint32_t get_opaque_length();
		char* get_opaque(uint32_t length);
};

#endif
