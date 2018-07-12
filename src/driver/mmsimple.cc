#include "driver/mmsimple.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

void MicroMoodySimple::sendCmd(unsigned char byte)
{
	txbuf[0] = byte;
	i2c.xmit(address, 3, txbuf, 0, txbuf);
}

void MicroMoodySimple::setBrightness(unsigned char red, unsigned char green)
{
	txbuf[0] = 13;
	txbuf[1] = green;
	txbuf[2] = red;
	i2c.xmit(address, 3, txbuf, 0, txbuf);
}

void MicroMoodySimple::off()         { sendCmd(0); }
void MicroMoodySimple::redOff()      { sendCmd(1); }
void MicroMoodySimple::greenOff()    { sendCmd(2); }
void MicroMoodySimple::blueOff()     { sendCmd(3); }
void MicroMoodySimple::redOn()       { sendCmd(4); }
void MicroMoodySimple::greenOn()     { sendCmd(5); }
void MicroMoodySimple::blueOn()      { sendCmd(6); }
void MicroMoodySimple::toggleRed()   { sendCmd(7); }
void MicroMoodySimple::toggleGreen() { sendCmd(8); }
void MicroMoodySimple::toggleBlue()  { sendCmd(9); }
void MicroMoodySimple::red()         { sendCmd(10); }
void MicroMoodySimple::green()       { sendCmd(11); }
void MicroMoodySimple::blue()        { sendCmd(12); }

MicroMoodySimple moody(0x11);
