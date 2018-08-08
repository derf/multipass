#ifndef EEPROM24LC64_H
#define EEPROM24LC64_H

class EEPROM24LC64 {
	private:
		EEPROM24LC64(const EEPROM24LC64 &copy);
		unsigned char const address;
		unsigned char txbuf[34];

	public:
		EEPROM24LC64(unsigned char const addr) : address(addr) {}

		void writePage(unsigned short addr, char *buf);
		void readPage(unsigned short addr, char *buf);
};

extern EEPROM24LC64 eeprom24lc64;

#endif
