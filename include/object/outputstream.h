/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <stdint.h>
#ifdef CONFIG_ostream
#include <ostream>
#endif

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
	OutputStream & operator<<(float number);
	OutputStream & operator<<(double number);
	OutputStream & operator<<(OutputStream & (*fun) (OutputStream &));

#ifdef CONFIG_ostream
	OutputStream & operator<<(std::string s);
#endif

	void setBase(uint8_t b);
	void printf_uint8(uint8_t num);
	void printf_float(float num);
};


OutputStream & endl(OutputStream & os);

OutputStream & bin(OutputStream & os);

OutputStream & oct(OutputStream & os);

OutputStream & dec(OutputStream & os);

OutputStream & hex(OutputStream & os);

OutputStream & flush(OutputStream & os);

OutputStream & term(OutputStream & os);

#endif //OUTPUTSTREAM_H
