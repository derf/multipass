#include "driver/mmsubstate.h"
#if defined(MULTIPASS_ARCH_HAS_I2C) && !defined(DRIVER_SOFTI2C)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif

void MicroMoodySubstate::setSubstates(unsigned char substateCount, unsigned char switchDuration, unsigned char power)
{
	txbuf[0] = substateCount;
	txbuf[1] = power;
	txbuf[2] = switchDuration;
	i2c.xmit(address, 3, txbuf, 0, txbuf);
}

void MicroMoodySubstate::sleep()
{
	setSubstates(1, 0, 0);
}

void MicroMoodySubstate::noSubstates(unsigned char power1, unsigned char power2)
{
	setSubstates(1, power1, power2);
}

void MicroMoodySubstate::twoSubstates(unsigned char switchDuration, unsigned char power)
{
	setSubstates(2, switchDuration, power);
}

void MicroMoodySubstate::fourSubstates(unsigned char switchDuration, unsigned char power)
{
	setSubstates(4, switchDuration, power);
}

void MicroMoodySubstate::eightSubstates(unsigned char switchDuration, unsigned char power)
{
	setSubstates(8, switchDuration, power);
}

MicroMoodySubstate moody(0x11);
