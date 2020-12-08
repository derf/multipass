/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef STANDARDOUTPUT_H
#define STANDARDOUTPUT_H

class StandardOutput {
	private:
		StandardOutput(const StandardOutput &copy);
		char digit_buffer[sizeof(long long) * 8];
		unsigned char base;

	public:
		StandardOutput() : base(10) {};
		void setup();

		void put(char c);
		void write(const char *s);
		void flush() {}
		void printf_uint8(unsigned char num);
		void printf_float(float num);

		StandardOutput & operator<<(char c);
		StandardOutput & operator<<(unsigned char c);
		StandardOutput & operator<<(unsigned short number);
		StandardOutput & operator<<(short number);
		StandardOutput & operator<<(unsigned int number);
		StandardOutput & operator<<(int number);
		StandardOutput & operator<<(unsigned long number);
		StandardOutput & operator<<(long number);
		StandardOutput & operator<<(unsigned long long number);
		StandardOutput & operator<<(long long number);
		StandardOutput & operator<<(float number);
		StandardOutput & operator<<(double number);
		StandardOutput & operator<<(void *pointer);
		StandardOutput & operator<<(const char *text);
		StandardOutput & operator<<(StandardOutput & (*fun) (StandardOutput &));

		void setBase(unsigned char b);
};


StandardOutput & endl(StandardOutput & os);

StandardOutput & bin(StandardOutput & os);

StandardOutput & oct(StandardOutput & os);

StandardOutput & dec(StandardOutput & os);

StandardOutput & hex(StandardOutput & os);

StandardOutput & flush(StandardOutput & os);

StandardOutput & term(StandardOutput & os);

extern StandardOutput kout;

#endif
