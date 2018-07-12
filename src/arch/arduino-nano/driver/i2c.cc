#include "driver/i2c.h"
#include <avr/io.h>

/*
 * Send an I2C (re)start condition and the EEPROM address in read mode. Returns
 * after it has been transmitted successfully.
 */
static signed char i2c_start_read(unsigned char addr)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
	if (!(TWSR & 0x18)) // 0x08 == START ok, 0x10 == RESTART ok
		return -1;

	// Note: The R byte ("... | 1") causes the TWI momodule to switch to
	// Master Receive mode
	TWDR = (addr << 1) | 1;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
	if (TWSR != 0x40) // 0x40 == SLA+R transmitted, ACK receveid
		return -2;

	return 0;
}

/*
 * Send an I2C (re)start condition and the EEPROM address in write mode.
 * Returns after it has been transmitted successfully.
 */
static signed char i2c_start_write(unsigned char addr)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
	if (!(TWSR & 0x18)) // 0x08 == START ok, 0x10 == RESTART ok
		return -1;

	TWDR = (addr<< 1) | 0;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while (!(TWCR & _BV(TWINT)));
	if (TWSR != 0x18) // 0x18 == SLA+W transmitted, ACK received
		return -2;

	return 0;
}

/*
 * Send an I2C stop condition.
 */
static signed char i2c_stop()
{
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

/*
 * Sends len bytes to the EEPROM. Note that this method does NOT
 * send I2C start or stop conditions.
 */
static signed char i2c_send(uint8_t len, uint8_t *data)
{
	uint8_t pos = 0;

	for (pos = 0; pos < len; pos++) {
		TWDR = data[pos];
		TWCR = _BV(TWINT) | _BV(TWEN);
		while (!(TWCR & _BV(TWINT)));
		if (TWSR != 0x28) // 0x28 == byte transmitted, ACK received
			return pos;
	}

	return pos;
}

/*
 * Receives len bytes from the EEPROM into data. Note that this method does
 * NOT send I2C start or stop conditions.
 */
static signed char i2c_receive(uint8_t len, uint8_t *data)
{
	uint8_t pos = 0;

	for (pos = 0; pos < len; pos++) {
		if (pos == len-1) {
			// Don't ACK the last byte
			TWCR = _BV(TWINT) | _BV(TWEN);
		} else {
			// Automatically send ACK
			TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		}
		while (!(TWCR & _BV(TWINT)));
		data[pos] = TWDR;
		/*
		 * No error handling here -- We send the acks, the EEPROM only
		 * supplies raw data, so there's no way of knowing whether it's still
		 * talking to us or we're just reading garbage.
		 */
	}

	return pos;
}

signed char I2C::setup()
{
	TWSR = 0;
	TWBR = ((F_CPU / 100000UL) - 16) / 2;

	return 0;
}

void I2C::scan(unsigned int *results)
{
	for (unsigned char address = 0; address < 128; address++) {
		if (i2c_start_read(address) == 0) {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
		}
	}
	i2c_stop();
}

signed char I2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;

	if (tx_len) {
		if (i2c_start_write(address) < 0) {
			return -1;
		}
		if (i2c_send(tx_len, tx_buf) < 0) {
			return -1;
		}
	}
	if (rx_len) {
		if (i2c_start_read(address) < 0) {
			return -1;
		}
		if (i2c_receive(rx_len, rx_buf) < 0) {
			return -1;
		}
	}

	i2c_stop();
	return 0;
}

I2C i2c;
