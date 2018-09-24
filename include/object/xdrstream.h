#ifndef XDRSTREAM_H
#define XDRSTREAM_H

#include <stdint.h>

class XDRStream {
 private:
	XDRStream(const XDRStream& copy);
	uint32_t next_array_len;

 public:
	XDRStream() : next_array_len(0) {}
	void setNextArrayLen(uint32_t len) {next_array_len = len;}

	virtual void put(char c) = 0;

	virtual void flush() {}

	XDRStream & operator<<(char c);
	XDRStream & operator<<(unsigned char c);
	XDRStream & operator<<(unsigned short number);
	XDRStream & operator<<(short number);
	XDRStream & operator<<(unsigned int number);
	XDRStream & operator<<(int number);
	XDRStream & operator<<(unsigned long number);
	XDRStream & operator<<(long number);
	XDRStream & operator<<(unsigned long long number);
	XDRStream & operator<<(long long number);
	XDRStream & operator<<(const char *text);
	XDRStream & operator<<(XDRStream & (*fun) (XDRStream &));
};


// FLUSH: flush XDRStream buffer
XDRStream & flush(XDRStream & os);

// TERM: zero-termination
XDRStream & term(XDRStream & os);

XDRStream & opaque(XDRStream & os);

#endif //OUTPUTSTREAM_H
