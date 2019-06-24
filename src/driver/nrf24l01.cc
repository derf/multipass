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
	gpio.input(NRF24L01_IRQ_PIN, true);
	gpio.output(NRF24L01_EN_PIN);
	gpio.output(NRF24L01_CS_PIN);
	gpio.write(NRF24L01_EN_PIN, 0);
	csnHigh();
	arch.delay_ms(5);

	// Reset NRF_CONFIG and enable 16-bit CRC.
	writeRegister(NRF_CONFIG, 0b00001100);

	// Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
	// WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
	// sizes must never be used. See documentation for a more complete explanation.
	setRetries(5, 10);

	// Reset value is MAX
	setPALevel(RF24_PA_MAX);

	setDataRate(RF24_1MBPS);

	toggleFeatures();
	writeRegister(FEATURE, 0);
	writeRegister(DYNPD, 0);
	dynamic_payloads_enabled = false;

	// Reset current status
	// Notice reset and flush is the last thing we do
	writeRegister(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

	// Set up default configuration.  Callers can always change it later.
	// This channel should be universally safe and not bleed over into adjacent
	// spectrum.
	setChannel(76);

	// Flush buffers
	flushRx();
	flushTx();

	maskIRQ(true, false, false);

	powerUp(); //Power up by default when begin() is called

	// Enable PTX, do not write CE high so radio will remain in standby I mode ( 130us max to transition to RX or TX instead of 1500us from powerUp )
	// PTX should use only 22uA of power
	writeRegister(NRF_CONFIG, (readRegister(NRF_CONFIG)) & ~(1 << PRIM_RX));
}

//Power up now. Radio will not power down unless instructed by MCU for config changes etc.
void Nrf24l01::powerUp(void)
{
	uint8_t cfg = readRegister(NRF_CONFIG);

	// if not powered up then power up and wait for the radio to initialize
	if (!(cfg & (1 << PWR_UP)))
	{
		writeRegister(NRF_CONFIG, cfg | (1 << PWR_UP));

		// For nRF24L01+ to go from power down mode to TX or RX mode it must first pass through stand-by mode.
		// There must be a delay of Tpd2stby (see Table 16.) after the nRF24L01+ leaves power down mode before
		// the CEis set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
		arch.delay_us(5);
	}
}

void Nrf24l01::setRetries(uint8_t delay, uint8_t count)
{
	writeRegister(SETUP_RETR, (delay & 0xf) << ARD | (count & 0xf) << ARC);
}

void Nrf24l01::setPALevel(uint8_t level)
{
	uint8_t setup = readRegister(RF_SETUP) & 0b11111000;

	if (level > 3)
	{									// If invalid level, go to max PA
		level = (RF24_PA_MAX << 1) + 1; // +1 to support the SI24R1 chip extra bit
	}
	else
	{
		level = (level << 1) + 1; // Else set level as requested
	}

	writeRegister(RF_SETUP, setup |= level); // Write it to the chip
}

bool Nrf24l01::setDataRate(Nrf24l01::rf24_datarate_e speed)
{
	bool result = false;
	uint8_t setup = readRegister(RF_SETUP);

	// HIGH and LOW '00' is 1Mbs - our default
	setup &= ~((1 << RF_DR_LOW) | (1 << RF_DR_HIGH));

	txRxDelay = 85;
	if (speed == RF24_250KBPS)
	{
		// Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
		// Making it '10'.
		setup |= (1 << RF_DR_LOW);
		txRxDelay = 155;
	}
	else
	{
		// Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
		// Making it '01'
		if (speed == RF24_2MBPS)
		{
			setup |= (1 << RF_DR_HIGH);
			txRxDelay = 65;
		}
	}
	writeRegister(RF_SETUP, setup);

	// Verify our result
	if (readRegister(RF_SETUP) == setup)
	{
		result = true;
	}
	return result;
}

void Nrf24l01::toggleFeatures(void)
{
	beginTransaction();
	txbuf[0] = ACTIVATE;
	txbuf[1] = 0x73;
	spi.xmit(2, txbuf, 0, rxbuf);
	endTransaction();
}

void Nrf24l01::setChannel(uint8_t channel)
{
	writeRegister(RF_CH, rf24_min(channel, 125));
}

uint8_t Nrf24l01::write(const void *buf, uint8_t len, bool blocking)
{
	writePayload(buf, len, W_TX_PAYLOAD);

	gpio.write(NRF24L01_EN_PIN, 1);
	arch.delay_us(10);
	gpio.write(NRF24L01_EN_PIN, 0);

	if (!blocking)
	{
		return 0;
	}

	while (!(getStatus() & ((1 << TX_DS) | (1 << MAX_RT))))
		;
	uint8_t status = writeRegister(NRF_STATUS, ((1 << TX_DS) | (1 << MAX_RT)));

	if (status & (1 << MAX_RT))
	{
		// flush_tx(); //Only going to be 1 packet int the FIFO at a time using this method, so just flush
		return 0;
	}
	return 1;
}

void Nrf24l01::maskIRQ(bool tx, bool fail, bool rx)
{

	uint8_t config = readRegister(NRF_CONFIG);
	/* clear the interrupt flags */
	config &= ~(1 << MASK_MAX_RT | 1 << MASK_TX_DS | 1 << MASK_RX_DR);
	/* set the specified interrupt flags */
	config |= fail << MASK_MAX_RT | tx << MASK_TX_DS | rx << MASK_RX_DR;
	writeRegister(NRF_CONFIG, config);
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
	txbuf[0] = R_REGISTER | (REGISTER_MASK & reg);
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

uint8_t Nrf24l01::writePayload(const void *buf, uint8_t data_len, const uint8_t writeType)
{
	data_len = rf24_min(data_len, payload_size);
	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

	//printf("[Writing %u bytes %u blanks]",data_len,blank_len);
	//IF_SERIAL_DEBUG( printf("[Writing %u bytes %u blanks]\n",data_len,blank_len); );

	beginTransaction();
	txbuf[0] = writeType;
	spi.xmit(1, txbuf, 1, rxbuf);
	spi.xmit(data_len, (unsigned char *)buf, 0, NULL);
	txbuf[0] = 0;
	while (blank_len--)
	{
		spi.xmit(1, txbuf, 0, NULL);
	}
	endTransaction();

	return rxbuf[0];
}

uint8_t Nrf24l01::flushRx(void)
{
	txbuf[0] = FLUSH_RX;
	beginTransaction();
	spi.xmit(1, txbuf, 1, rxbuf);
	endTransaction();
	return rxbuf[0];
}

/****************************************************************************/

uint8_t Nrf24l01::flushTx(void)
{
	txbuf[0] = FLUSH_TX;
	beginTransaction();
	spi.xmit(1, txbuf, 1, rxbuf);
	endTransaction();
	return rxbuf[0];
}

Nrf24l01 nrf24l01;