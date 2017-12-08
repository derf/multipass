#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <stdint.h>

class OutputStream {
 private:
	OutputStream(const OutputStream& copy);

	char digit_buffer[sizeof(long long) * 8];
	uint8_t base;

 public:
	OutputStream();

	virtual void put(char c) = 0;

	virtual void write(const char *s) {
		while (*s) {
			put(*s++);
		}
	}

	virtual void flush() {}

	OutputStream & operator<<(char c);
	OutputStream & operator<<(unsigned char c);
	OutputStream & operator<<(unsigned short number);
	OutputStream & operator<<(short number);
	OutputStream & operator<<(unsigned int number);
	OutputStream & operator<<(int number);
	OutputStream & operator<<(unsigned long number);
	OutputStream & operator<<(long number);
	OutputStream & operator<<(unsigned long long number);
	OutputStream & operator<<(long long number);
	OutputStream & operator<<(void *pointer);
	OutputStream & operator<<(const char *text);
	OutputStream & operator<<(OutputStream & (*fun) (OutputStream &));

	void setBase(uint8_t b);
};


// ENDL: new line character (and flush)
OutputStream & endl(OutputStream & os);

// BIN: print numbers in binary form.
OutputStream & bin(OutputStream & os);

// OCT: print numbers in octal form.
OutputStream & oct(OutputStream & os);

// DEC: print numbers in decimal form.
OutputStream & dec(OutputStream & os);

// HEX: print numbers in hexadecimal form.
OutputStream & hex(OutputStream & os);

// FLUSH: flush OutputStream buffer
OutputStream & flush(OutputStream & os);

// TERM: zero-termination
OutputStream & term(OutputStream & os);

#endif //OUTPUTSTREAM_H
