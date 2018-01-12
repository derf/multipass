#ifndef MMSIMPLE_H
#define MMSIMPLE_H

class MicroMoodySimple {
	private:
		MicroMoodySimple(const MicroMoodySimple &copy);

		unsigned char const address;
		unsigned char txbuf[3];

		void sendCmd(unsigned char byte);

	public:
		MicroMoodySimple(unsigned char const addr) : address(addr) {}

		void off();
		void red();
		void redOn();
		void redOff();
		void green();
		void greenOn();
		void greenOff();
		void blue();
		void blueOn();
		void blueOff();
		void toggleRed();
		void toggleGreen();
		void toggleBlue();
		void setBrightness(unsigned char red, unsigned char green);
};

extern MicroMoodySimple moody;

#endif
