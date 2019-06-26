#ifndef XDRSTREAM_H
#define XDRSTREAM_H

#include <stdint.h>

class XDRStream {
 private:
	XDRStream(const XDRStream& copy);
	uint16_t next_array_len;
	bool is_fixed_length;

 public:
	XDRStream() : next_array_len(0) {}
	void setNextArrayLen(uint16_t len) {next_array_len = len;}
	void setFixedLength() { is_fixed_length = true; }
	void setVariableLength() { is_fixed_length = false; }

	virtual void put(char c) = 0;

	virtual void flush() {}

	XDRStream & operator<<(char c);
	XDRStream & operator<<(unsigned char c);
	XDRStream & operator<<(uint16_t number);
	XDRStream & operator<<(int16_t number);
	XDRStream & operator<<(uint32_t number);
	XDRStream & operator<<(int32_t number);
	XDRStream & operator<<(uint64_t number);
	XDRStream & operator<<(int64_t number);
	XDRStream & operator<<(float number);
	XDRStream & operator<<(double number);
	XDRStream & operator<<(char const *text);
	template<uint16_t TSize> XDRStream & operator<<(char const (&text)[TSize]);
	XDRStream & operator<<(XDRStream & (*fun) (XDRStream &));
};


// FLUSH: flush XDRStream buffer
XDRStream & flush(XDRStream & os);

// TERM: zero-termination
XDRStream & term(XDRStream & os);

template<int TSize>
XDRStream & opaque(XDRStream & os);

XDRStream & fixed(XDRStream & os);
XDRStream & variable(XDRStream & os);

#endif //OUTPUTSTREAM_H
