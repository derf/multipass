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
	os_printf("%u", number);
	return *this;
}

StandardOutput & StandardOutput::operator<<(short number)
{
	os_printf("%d", number);
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned int number)
{
	os_printf("%u", number);
	return *this;
}

StandardOutput & StandardOutput::operator<<(int number)
{
	os_printf("%d", number);
	return *this;
}

StandardOutput & StandardOutput::operator<<(unsigned long number)
{
	os_printf("%lu", number);
	return *this;
}

StandardOutput & StandardOutput::operator<<(long number)
{
	os_printf("%ld", number);
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
	write(text);
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

static inline char format_hex_nibble(uint8_t num)
{
	if (num > 9) {
		return 'a' + num - 10;
	}
	return '0' + num;
}

void StandardOutput::printf_uint8(uint8_t num)
{
	put(format_hex_nibble(num / 16));
	put(format_hex_nibble(num % 16));
}

void StandardOutput::printf_float(float num)
{
	if (num < 0) {
		put('-');
		num *= -1;
	}
	if (num > 1000) {
		put('0' + (((int)num % 10000) / 1000));
	}
	if (num > 100) {
		put('0' + (((int)num % 1000) / 100));
	}
	if (num > 10) {
		put('0' + (((int)num % 100) / 10));
	}
	put('0' + ((int)num % 10));
	put('.');
	put('0' + ((int)(num * 10) % 10));
	put('0' + ((int)(num * 100) % 10));
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
