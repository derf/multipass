#ifndef XDR16_H
#define XDR16_H

#include <stdint.h>

class XDRWriter
{
private:
	XDRWriter(const XDRWriter &copy);
	char *buffer;
	uint16_t next_array_len;
	uint16_t pos;
	bool is_fixed_length;

public:
	XDRWriter(char *output_buf) : buffer(output_buf), next_array_len(0), pos(0) {}

	inline void setNextArrayLen(uint16_t len) { next_array_len = len; }
	inline void setFixedLength() { is_fixed_length = true; }
	inline void setVariableLength() { is_fixed_length = false; }
	inline void startList()
	{
		if (!is_fixed_length)
		{
			put(next_array_len);
		}
	}

	inline void put(char c) { put((int16_t)c); }
	inline void put(unsigned char c) { put((uint16_t)c); }
	void put(uint16_t number);
	void put(int16_t number);
	void put(uint32_t number);
	void put(int32_t number);
	void put(uint64_t number);
	void put(int64_t number);
	void put(float number);
	void put(double number);
	void put(char const *text);
	template <uint16_t TSize>
	void put(char const (&text)[TSize]);

	inline uint16_t size() { return pos; }
};

class XDRReader
{
private:
	XDRReader(const XDRReader &copy);
	char *data;
	uint32_t pos;

public:
	XDRReader(char *d) : pos(0) { data = d; }
	int16_t get_int16();
	uint16_t get_uint16();
	uint32_t get_uint32();
	int32_t get_int32();
	uint64_t get_uint64();
	int64_t get_int64();
	float get_float();
	double get_double();
	uint32_t get_opaque_length();
	char *get_opaque(uint32_t length);
	void get_string(char *target);
};

#endif
