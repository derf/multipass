#ifndef AM2320_H
#define AM2320_H

class AM2320 {
	private:
		AM2320(const AM2320 &copy);
		unsigned char const address;
		unsigned char txbuf[3];
		unsigned char rxbuf[8];

	public:
		AM2320(unsigned char const addr) : address(addr) {}

		void read();
		unsigned char getStatus();
		float getTemp();
		float getHumidity();
};

extern AM2320 am2320;

#endif
