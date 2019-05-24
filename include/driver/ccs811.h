#ifndef CCS811_H
#define CCS811_H

class CCS811 {
	private:
		CCS811(const CCS811 &copy);
		unsigned char const address;
		unsigned char txbuf[3];
		unsigned char rxbuf[2];

	public:
		CCS811(unsigned char const addr) : address(addr) {}

		void init();
		short check();
};

extern CCS811 ccs811;

#endif
