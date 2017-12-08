#include "driver/stdout.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
void uart_div_modify(uint8_t uart_no, uint32 DivLatchValue);
void os_printf_plus(const char *s, ...);
}

StandardOutput & StandardOutput::operator<<(unsigned char c)
{
	put(c);
	return *this;
}

StandardOutput & StandardOutput::operator<<(char c)
{
	put(c);
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned short number)
{
	*this << (unsigned long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(short number)
{
	*this << (long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned int number)
{
	*this << (unsigned long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(int number)
{
	*this << (long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned long number)
{
	*this << (unsigned long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(long number)
{
	*this << (long long)number;
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned long long number)
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

StandardOutput & StandardOutput::operator<<(long long number)
{
	if (number < 0) {
		put('-');
		number *= -1;
	}
	*this << (unsigned long long)number;

	return *this;
}

StandardOutput & StandardOutput::operator<<(void *pointer)
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

StandardOutput & StandardOutput::operator<<(const char *text)
{
	int i = 0;
	while (text[i] != '\0') {
		put(text[i++]);
	}
	return *this;
}

StandardOutput & StandardOutput::operator<<(StandardOutput & (*fkt) (StandardOutput &))
{
	return fkt(*this);
}

void StandardOutput::setBase(uint8_t b)
{
	if (b == 2 || b == 8 || b == 10 || b == 16) {
		base = b;
	}
}

// FLUSH
StandardOutput & flush(StandardOutput & os)
{
	os.flush();
	return os;
}

// ENDL: fuegt einen Zeilenumbruch in die Ausgabe ein.
StandardOutput & endl(StandardOutput & os)
{
	os.put('\n');
	os.flush();
	return os;
}

// BIN: print numbers in binary form
StandardOutput & bin(StandardOutput & os)
{
	os.setBase(2);
	return os;
}

// OCT: print numbers in octal form.
StandardOutput & oct(StandardOutput & os)
{
	os.setBase(8);
	return os;
}

// DEC: print numbers in decimal form.
StandardOutput & dec(StandardOutput & os)
{
	os.setBase(10);
	return os;
}

// HEX: print numbers in hexadecimal form.
StandardOutput & hex(StandardOutput & os)
{
	os.setBase(16);
	return os;
}

// TERM: null-termination
StandardOutput & term(StandardOutput & os)
{
	os.put('\0');
	os.flush();
	return os;
}

StandardOutput::StandardOutput()
{
	base = 10;
}

void StandardOutput::setup()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
}

void StandardOutput::put(char c)
{
	os_printf("%c", c);
}

void StandardOutput::write(const char *s)
{
	os_printf("%s", s);
}

StandardOutput kout;
