/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "object/outputstream.h"

OutputStream & OutputStream::operator<<(unsigned char c)
{
	*this << (unsigned long long)c;
	return *this;
}

OutputStream & OutputStream::operator<<(char c)
{
	put(c);
	return *this;
}

OutputStream & OutputStream::operator<<(unsigned short number)
{
	*this << (unsigned long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(short number)
{
	*this << (long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(unsigned int number)
{
	*this << (unsigned long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(int number)
{
	*this << (long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(unsigned long number)
{
	*this << (unsigned long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(long number)
{
	*this << (long long)number;
	return *this;
}

OutputStream & OutputStream::operator<<(unsigned long long number)
{
	switch (base) {
	case 2:
		put('0');
		put('b');
		break;
	case 8:
		put('0');
		break;
	case 16:
		if (number < 16) {
			put('0');
		}
		break;
	}

	if (number == 0) {
		put('0');
		return *this;
	}

	signed int i = 0;
	while (number > 0) {
		if (base == 16 && number % base > 9) {
			digit_buffer[i] = 'a' + (number % base) - 10;
		} else {
			digit_buffer[i] = '0' + (number % base);
		}
		number /= base;
		i++;
	}
	i--;
	for (; i >= 0; i--) {
		put(digit_buffer[i]);
	}
	return *this;

}

OutputStream & OutputStream::operator<<(long long number)
{
	if (number < 0) {
		put('-');
		number *= -1;
	}
	*this << (unsigned long long)number;

	return *this;
}

OutputStream & OutputStream::operator<<(double number)
{
	*this << (float)number;
	return *this;
}

OutputStream & OutputStream::operator<<(float number)
{
	printf_float(number);
	return *this;
}

OutputStream & OutputStream::operator<<(void *pointer)
{
	unsigned char temp_base = base;
	*this << hex << (long)pointer;
	switch (temp_base) {
	case 2:
		*this << bin; break;
	case 8:
		*this << oct; break;
	case 10:
		*this << dec; break;
	}
	return *this;
}

OutputStream & OutputStream::operator<<(const char *text)
{
	int i = 0;
	while (text[i] != '\0') {
		put(text[i++]);
	}
	return *this;
}

OutputStream & OutputStream::operator<<(OutputStream & (*fkt) (OutputStream &))
{
	return fkt(*this);
}

#ifdef WITH_OSTREAM
OutputStream & OutputStream::operator<<(std::string s)
{
	for (auto c : s) {
		put(c);
	}
	return *this;
}
#endif

void OutputStream::setBase(uint8_t b)
{
	if (b == 2 || b == 8 || b == 10 || b == 16) {
		base = b;
	}
}

static inline char format_hex_nibble(uint8_t num)
{
	if (num > 9) {
		return 'a' + num - 10;
	}
	return '0' + num;
}

void OutputStream::printf_uint8(uint8_t num)
{
	put(format_hex_nibble(num / 16));
	put(format_hex_nibble(num % 16));
}

void OutputStream::printf_float(float num)
{
	if (num < 0) {
		put('-');
		num *= -1;
	}
	if (num > 1000) {
		put('0' + (((unsigned int)num % 10000) / 1000));
	}
	if (num > 100) {
		put('0' + (((unsigned int)num % 1000) / 100));
	}
	if (num > 10) {
		put('0' + (((unsigned int)num % 100) / 10));
	}
	put('0' + ((unsigned int)num % 10));
	put('.');
	put('0' + ((unsigned int)(num * 10) % 10));
	put('0' + ((unsigned int)(num * 100) % 10));
}

OutputStream & flush(OutputStream & os)
{
	os.flush();
	return os;
}

OutputStream & endl(OutputStream & os)
{
	os.put('\n');
	os.flush();
	return os;
}

OutputStream & bin(OutputStream & os)
{
	os.setBase(2);
	return os;
}

OutputStream & oct(OutputStream & os)
{
	os.setBase(8);
	return os;
}

OutputStream & dec(OutputStream & os)
{
	os.setBase(10);
	return os;
}

OutputStream & hex(OutputStream & os)
{
	os.setBase(16);
	return os;
}

OutputStream & term(OutputStream & os)
{
	os.put('\0');
	os.flush();
	return os;
}

OutputStream::OutputStream()
{
    base = 10;
}
