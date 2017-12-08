#include "object/outputstream.h"

OutputStream & OutputStream::operator<<(unsigned char c)
{
	put(c);
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
		put('0');
		put('x');
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

OutputStream & OutputStream::operator<<(void *pointer)
{
	unsigned short temp_base = base;
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

void OutputStream::setBase(uint8_t b)
{
	if (b == 2 || b == 8 || b == 10 || b == 16) {
		base = b;
	}
}

// FLUSH
OutputStream & flush(OutputStream & os)
{
	os.flush();
	return os;
}

// ENDL: fuegt einen Zeilenumbruch in die Ausgabe ein.
OutputStream & endl(OutputStream & os)
{
	os.put('\n');
	os.flush();
	return os;
}

// BIN: print numbers in binary form
OutputStream & bin(OutputStream & os)
{
	os.setBase(2);
	return os;
}

// OCT: print numbers in octal form.
OutputStream & oct(OutputStream & os)
{
	os.setBase(8);
	return os;
}

// DEC: print numbers in decimal form.
OutputStream & dec(OutputStream & os)
{
	os.setBase(10);
	return os;
}

// HEX: print numbers in hexadecimal form.
OutputStream & hex(OutputStream & os)
{
	os.setBase(16);
	return os;
}

// TERM: null-termination
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
