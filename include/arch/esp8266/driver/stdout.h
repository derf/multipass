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


// ENDL: new line character (and flush)
StandardOutput & endl(StandardOutput & os);

// BIN: print numbers in binary form.
StandardOutput & bin(StandardOutput & os);

// OCT: print numbers in octal form.
StandardOutput & oct(StandardOutput & os);

// DEC: print numbers in decimal form.
StandardOutput & dec(StandardOutput & os);

// HEX: print numbers in hexadecimal form.
StandardOutput & hex(StandardOutput & os);

// FLUSH: flush StandardOutput buffer
StandardOutput & flush(StandardOutput & os);

// TERM: zero-termination
StandardOutput & term(StandardOutput & os);

extern StandardOutput kout;

#endif
