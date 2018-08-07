#include "driver/soft_i2c.h"
#include "driver/gpio.h"
#include "arch.h"

#ifdef SOFTI2C_PULLUP
#define SDA_HIGH gpio.input(sda, 1)
#define SDA_LOW gpio.output(sda, 0)
#define SCL_HIGH gpio.input(scl, 1)
#define SCL_LOW gpio.output(scl, 0)
#else
#define SDA_HIGH gpio.input(sda)
#define SDA_LOW gpio.output(sda)
#define SCL_HIGH gpio.input(scl)
#define SCL_LOW gpio.output(scl)
#endif

signed char SoftI2C::setup()
{
	SDA_HIGH;
	SCL_HIGH;
	return 0;
}

void SoftI2C::start()
{
	SDA_HIGH;
	SCL_HIGH;
	//
	SDA_LOW;
	//
	SCL_LOW;
}

void SoftI2C::stop()
{
	SCL_LOW;
	//
	SDA_LOW;
	//
	SCL_HIGH;
	//
	SDA_HIGH;
}

bool SoftI2C::tx(unsigned char byte)
{
	unsigned char got_ack = 0;
	for (unsigned char i = 0; i <= 8; i++) {
		if ((byte & 0x80) || (i == 8)) {
			SDA_HIGH;
		} else {
			SDA_LOW;
		}
		byte <<= 1;
		//
		SCL_HIGH;
		//
		if (i == 8) {
			if (!gpio.read(sda)) {
				got_ack = 1;
			}
		}
		SCL_LOW;
		//
	}
	return got_ack;
}

unsigned char SoftI2C::rx(bool send_ack)
{
	unsigned char byte = 0;
	SDA_HIGH;
	for (unsigned char i = 0; i <= 8; i++) {
		//
		SCL_HIGH;
		//
		if ((i < 8) && gpio.read(sda)) {
			byte |= 1 << (7 - i);
		}
		//
		SCL_LOW;
		//
		if ((i == 7) && send_ack) {
			SDA_LOW;
		} else if ((i == 8) && send_ack) {
			SDA_HIGH;
		}
	}
	return byte;
}

void SoftI2C::scan(unsigned int *results)
{
	unsigned char i2caddr;
	for (unsigned char address = 0; address < 128; address++) {

		i2caddr = (address << 1) | 0;

		start();

		if (tx(i2caddr)) {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
			stop();
		}
	}
	stop();
}

signed char SoftI2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;

	if (tx_len) {
		start();
		tx((address << 1) | 0);

		for (i = 0; i < tx_len; i++) {
			tx(tx_buf[i]);
		}
	}
	if (rx_len) {
		start();
		tx((address << 1) | 1);

		for (i = 1; i <= rx_len; i++) {
			rx_buf[i-1] = rx((i < rx_len) * 1);
		}
	}

	stop();

	return 0;
}

#ifdef MULTIPASS_ARCH_esp8266
SoftI2C i2c(GPIO::d7, GPIO::d8);
#elif MULTIPASS_ARCH_arduino_nano
SoftI2C i2c(GPIO::pc5, GPIO::pc4);
#elif MULTIPASS_ARCH_msp430fr5969lp
SoftI2C i2c(GPIO::p1_6, GPIO::p1_7);
#endif
