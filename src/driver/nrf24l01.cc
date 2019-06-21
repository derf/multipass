/*
 * Based on https://github.com/nRF24/RF24
 */

#include <stdlib.h>

#include "driver/nrf24l01.h"
#include "driver/nrf24l01/registers.h"
#include "driver/spi_b.h"
#include "driver/gpio.h"
#include "arch.h"

#ifndef NRF24L01_EN_PIN
#error makeflag nrf24l01_en_pin required
#endif

#ifndef NRF24L01_CS_PIN
#error makeflag nrf24l01_cs_pin required
#endif

void Nrf24l01::setup()
{
	spi.setup();
	gpio.output(NRF24L01_EN_PIN);
	gpio.output(NRF24L01_CS_PIN);
	gpio.write(NRF24L01_EN_PIN, 0);
	csnHigh();
	arch.delay_ms(5);

	// Reset NRF_CONFIG and enable 16-bit CRC.
	writeRegister( NRF_CONFIG, 0b00001100 ) ;

	// Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
	// WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
	// sizes must never be used. See documentation for a more complete explanation.
	setRetries(5,15);

	// Reset value is MAX
	setPALevel( RF24_PA_MAX ) ;

}

void Nrf24l01::setRetries(uint8_t delay, uint8_t count)
{
	writeRegister(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

void Nrf24l01::setPALevel(uint8_t level)
{
	uint8_t setup = readRegister(RF_SETUP) & 0b11111000;

	if(level > 3){  						// If invalid level, go to max PA
		level = (RF24_PA_MAX << 1) + 1;		// +1 to support the SI24R1 chip extra bit
	}else{
		level = (level << 1) + 1;	 		// Else set level as requested
	}

	writeRegister( RF_SETUP, setup |= level ) ;	// Write it to the chip
}

uint8_t Nrf24l01::getStatus()
{
	txbuf[0] = NOP;
	beginTransaction();
	spi.xmit(1, txbuf, 1, rxbuf);
	endTransaction();

	return rxbuf[0];
}

uint8_t Nrf24l01::readRegister(uint8_t reg)
{
	txbuf[0] = R_REGISTER | ( REGISTER_MASK & reg );
	txbuf[1] = NOP;

	beginTransaction();
	spi.xmit(2, txbuf, 2, rxbuf);
	endTransaction();

	return rxbuf[1];
}

uint8_t Nrf24l01::writeRegister(uint8_t reg, uint8_t value)
{
	txbuf[0] = W_REGISTER | (REGISTER_MASK & reg);
	txbuf[1] = value;

	beginTransaction();
	spi.xmit(2, txbuf, 1, rxbuf);
	endTransaction();

	return rxbuf[0];
}

Nrf24l01 nrf24l01;
