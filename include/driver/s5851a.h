#ifndef S5851A_H
#define S5851A_H

class S5851A {
	private:
		S5851A(const S5851A &copy);
		unsigned char const address;
		unsigned char txbuf[1];
		unsigned char rxbuf[2];

	public:
		S5851A(unsigned char const addr) : address(addr) {}

		float getTemp();
};

extern S5851A s5851a;

#endif
