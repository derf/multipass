/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/i2c.h"
#include "arch.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_I2C
#define F_I2C 100000UL
#endif

inline void await_twint(unsigned char twcr_values)
{
#if 1
	TWCR = twcr_values | _BV(TWINT) | _BV(TWIE);
	while (!(TWCR & _BV(TWINT))) {
		arch.idle();
	}
#endif
#if 0
	TWCR = twcr_values | _BV(TWINT);
	while (!(TWCR & _BV(TWINT))) ;
#endif
}

/*
 * Send an I2C (re)start condition and the EEPROM address in read mode. Returns
 * after it has been transmitted successfully.
 */
static signed char i2c_start_read(unsigned char addr)
{
	await_twint(_BV(TWSTA) | _BV(TWEN));
	if (!(TWSR & 0x18)) // 0x08 == START ok, 0x10 == RESTART ok
		return -1;

	// Note: An adress with read bit set ("... | 1") causes the TWI module
	// to switch to Master Receive mode
	TWDR = (addr << 1) | 1;
	await_twint(_BV(TWEN));
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
	await_twint(_BV(TWSTA) | _BV(TWEN));
	if (!(TWSR & 0x18)) // 0x08 == START ok, 0x10 == RESTART ok
		return -1;

	TWDR = (addr<< 1) | 0;
	await_twint(_BV(TWEN));
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
	return 0;
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
		await_twint(_BV(TWEN));
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
		await_twint(_BV(TWEN) | ( _BV(TWEA) * (pos < len-1) ) );
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
	TWBR = ((F_CPU / F_I2C) - 16) / 2;

	return 0;
}

void I2C::scan(unsigned int *results)
{
	for (unsigned char address = 0; address < 128; address++) {
		if (i2c_start_read(address) == 0) {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
			i2c_stop();
		}
	}
	i2c_stop();
}

signed char I2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
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

ISR(TWI_vect)
{
}
