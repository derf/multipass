#include "driver/lm75.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

float LM75::getTemp()
{
	txbuf[0] = 0;
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	i2c.xmit(address, 1, txbuf, 2, rxbuf);

	return rxbuf[0] + (rxbuf[1] / 256.0);
}

void LM75::setOS(unsigned char os)
{
	txbuf[0] = 0x03;
	txbuf[1] = os;
	txbuf[2] = 0;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
}

void LM75::setHyst(unsigned char hyst)
{
	txbuf[0] = 0x02;
	txbuf[1] = hyst;
	txbuf[2] = 0;
	i2c.xmit(address, 3, txbuf, 0, rxbuf);
}

LM75 lm75(0x48);
