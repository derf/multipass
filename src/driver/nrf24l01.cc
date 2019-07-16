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

static const uint8_t child_pipe[] =
	{
		RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5};
static const uint8_t child_payload_size[] =
	{
		RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5};
static const uint8_t child_pipe_enable[] =
	{
		ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5};

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
/*
int Nrf24l01::init(uint8_t addr, uint8_t channel, rf24_datarate_e datarate){
    
  if(channel > 125){
   return -1; 
  }
  
  unsigned char node_addr[5] = {addr, 'x', 'S', 'D', 'P'};
  
  setAutoAck(1);
  enableAckPayload();
  maskIRQ(true, true, false);
  enableDynamicPayloads();  
  setPALevel(RF24_PA_MAX);
  setChannel(channel);
  setDataRate(datarate);
  openWritingPipe((const uint8_t*)GATEWAY_NAME); // GATEWAY_NAME is defined in the MSP430FR5969 RF24_arch_config.h
  currentWritingPipe = 0;
  openReadingPipe(1, node_addr);
#ifdef CONFIG_Apps_SolarDoorplate_PacketHandler
  node_addr[0] = CONFIG_PACKETHANDLER_BROADCAST_ID;
  openReadingPipe(2, node_addr);
#endif  
  if(getChannel() != channel){
   return -2; 
  }
  
  if(getDataRate() != datarate){
   return -3; 
  }
  
  return 0;
  
}
*/
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

void Nrf24l01::powerDown(void)
{
	gpio.write(NRF24L01_EN_PIN, 0);
	writeRegister(NRF_CONFIG, readRegister(NRF_CONFIG) & ~(1 << PWR_UP));
}

void Nrf24l01::setRetries(uint8_t delay, uint8_t count)
{
	writeRegister(SETUP_RETR, (delay & 0xf) << ARD | (count & 0xf) << ARC);
}

void Nrf24l01::setPayloadSize(uint8_t size)
{
	payload_size = rf24_min(size, 32);
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

void Nrf24l01::enableAckPayload(void)
{
	//
	// enable ack payload and dynamic payload features
	//

	//toggle_features();
	writeRegister(FEATURE, readRegister(FEATURE) | (1 << EN_ACK_PAY) | (1 << EN_DPL));

	//IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",read_register(FEATURE)));

	//
	// Enable dynamic payload on pipes 0 & 1
	//

	writeRegister(DYNPD, readRegister(DYNPD) | (1 << DPL_P1) | (1 << DPL_P0));
	dynamic_payloads_enabled = true;
}

void Nrf24l01::setChannel(uint8_t channel)
{
	writeRegister(RF_CH, rf24_min(channel, 125));
}

uint8_t Nrf24l01::write(const void *buf, uint8_t len, bool await_ack, bool blocking)
{
	writePayload(buf, len, await_ack ? W_TX_PAYLOAD : W_TX_PAYLOAD_NO_ACK);

	gpio.write(NRF24L01_EN_PIN, 1);

	if (!blocking)
	{
		arch.delay_us(10);
		gpio.write(NRF24L01_EN_PIN, 0);
		return 0;
	}

	while (!(getStatus() & ((1 << TX_DS) | (1 << MAX_RT))))
		;
	gpio.write(NRF24L01_EN_PIN, 1);
	uint8_t status = writeRegister(NRF_STATUS, ((1 << TX_DS) | (1 << MAX_RT)));

	if (status & (1 << MAX_RT))
	{
		flushTx();
		return 0;
	}
	return 1;
}

void Nrf24l01::startListening(void)
{
	writeRegister(NRF_CONFIG, readRegister(NRF_CONFIG) | (1 << PRIM_RX));
	writeRegister(NRF_STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
	gpio.write(NRF24L01_EN_PIN, 1);

	// Restore the pipe0 adddress, if exists
	if (pipe0_reading_address[0] > 0)
	{
		writeRegister(RX_ADDR_P0, pipe0_reading_address, addr_width);
	}
	else
	{
		closeReadingPipe(0);
	}

	if (readRegister(FEATURE) & (1 << EN_ACK_PAY))
	{
		flushTx();
	}
}

void Nrf24l01::stopListening(void)
{
	gpio.write(NRF24L01_EN_PIN, 0);

	arch.delay_us(txRxDelay);

	if (readRegister(FEATURE) & (1 << EN_ACK_PAY))
	{
		arch.delay_us(txRxDelay); //200
		flushTx();
	}
	//flush_rx();
	writeRegister(NRF_CONFIG, (readRegister(NRF_CONFIG)) & ~(1 << PRIM_RX));

	writeRegister(EN_RXADDR, readRegister(EN_RXADDR) | (1 << child_pipe_enable[0])); // Enable RX on pipe0
}

bool Nrf24l01::available(void)
{
	return available(NULL);
}

/****************************************************************************/

bool Nrf24l01::available(uint8_t *pipe_num)
{
	if (!(readRegister(FIFO_STATUS) & (1 << RX_EMPTY)))
	{

		// If the caller wants the pipe number, include that
		if (pipe_num)
		{
			uint8_t status = getStatus();
			*pipe_num = (status >> RX_P_NO) & 0b111;
		}
		return 1;
	}
	return 0;
}

bool Nrf24l01::testCarrier(void)
{
	return (readRegister(CD) & 1);
}

/****************************************************************************/

bool Nrf24l01::testRPD(void)
{
	return (readRegister(RPD) & 1);
}

void Nrf24l01::openReadingPipe(uint8_t child, const uint8_t *address)
{
	// If this is pipe 0, cache the address.  This is needed because
	// openWritingPipe() will overwrite the pipe 0 address, so
	// startListening() will have to restore it.
	if (child == 0)
	{
		pipe0_reading_address[0] = address[0];
		pipe0_reading_address[1] = address[1];
		pipe0_reading_address[2] = address[2];
		pipe0_reading_address[3] = address[3];
		pipe0_reading_address[4] = address[4];
	}
	if (child <= 6)
	{
		// For pipes 2-5, only write the LSB
		if (child < 2)
		{
			writeRegister(child_pipe[child], address, addr_width);
		}
		else
		{
			writeRegister(child_pipe[child], address, 1);
		}
		writeRegister(child_payload_size[child], payload_size);

		// Note it would be more efficient to set all of the bits for all open
		// pipes at once.  However, I thought it would make the calling code
		// more simple to do it this way.
		writeRegister(EN_RXADDR, readRegister(EN_RXADDR) | (1 << child_pipe_enable[child]));
	}
}

/****************************************************************************/

void Nrf24l01::closeReadingPipe(uint8_t pipe)
{
	writeRegister(EN_RXADDR, readRegister(EN_RXADDR) & ~(1 << child_pipe_enable[pipe]));
}

/****************************************************************************/
void Nrf24l01::openWritingPipe(const uint8_t *address)
{
	// Note that AVR 8-bit uC's store this LSB first, and the NRF24L01(+)
	// expects it LSB first too, so we're good.

	writeRegister(RX_ADDR_P0, address, addr_width);
	writeRegister(TX_ADDR, address, addr_width);

	//const uint8_t max_payload_size = 32;
	//write_register(RX_PW_P0,rf24_min(payload_size,max_payload_size));
	writeRegister(RX_PW_P0, payload_size);
}

void Nrf24l01::setAddressWidth(uint8_t a_width)
{
	if (a_width -= 2)
	{
		writeRegister(SETUP_AW, a_width % 4);
		addr_width = (a_width % 4) + 2;
	}
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

uint8_t Nrf24l01::writeRegister(uint8_t reg, const uint8_t *buf, uint8_t len)
{
	txbuf[0] = W_REGISTER | (REGISTER_MASK & reg);

	beginTransaction();
	spi.xmit(1, txbuf, 1, rxbuf);
	spi.xmit(len, (unsigned char *)buf, 0, NULL);
	endTransaction();

	return rxbuf[0];
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

uint8_t Nrf24l01::readPayload(void *buf, uint8_t data_len)
{
	uint8_t status;
	uint8_t *current = reinterpret_cast<uint8_t *>(buf);

	if (data_len > payload_size)
		data_len = payload_size;
	uint8_t blank_len = dynamic_payloads_enabled ? 0 : payload_size - data_len;

	beginTransaction();
	txbuf[0] = R_RX_PAYLOAD;
	spi.xmit(1, txbuf, 1, rxbuf);
	status = rxbuf[0];
	txbuf[0] = 0xf;
	;
	while (data_len--)
	{
		spi.xmit(1, txbuf, 1, rxbuf);
		*current++ = rxbuf[0];
	}
	while (blank_len--)
	{
		spi.xmit(1, txbuf, 1, rxbuf);
	}
	endTransaction();

	return status;
}

void Nrf24l01::read(void *buf, uint8_t len)
{

	// Fetch the payload
	readPayload(buf, len);

	//Clear the two possible interrupt flags with one command
	writeRegister(NRF_STATUS, (1 << RX_DR) | (1 << MAX_RT) | (1 << TX_DS));
}

void Nrf24l01::setDynamicPayloads(const bool enabled)
{
	if (enabled)
	{
		writeRegister(FEATURE, readRegister(FEATURE) | (1 << EN_DPL));
		writeRegister(DYNPD, readRegister(DYNPD) | (1 << DPL_P5) | (1 << DPL_P4) | (1 << DPL_P3) | (1 << DPL_P2) | (1 << DPL_P1) | (1 << DPL_P0));
	}
	else
	{
		writeRegister(FEATURE, readRegister(FEATURE) & ~(1 << EN_DPL));
		writeRegister(DYNPD, readRegister(DYNPD) & ~((1 << DPL_P5) | (1 << DPL_P4) | (1 << DPL_P3) | (1 << DPL_P2) | (1 << DPL_P1) | (1 << DPL_P0)));
	}

	dynamic_payloads_enabled = enabled;
}

void Nrf24l01::setDynamicAck(const bool enabled)
{
	if (enabled)
	{
		writeRegister(FEATURE, readRegister(FEATURE) | (1 << EN_DYN_ACK));
	}
	else
	{
		writeRegister(FEATURE, readRegister(FEATURE) & ~(1 << EN_DYN_ACK));
	}
}

void Nrf24l01::setAutoAck(bool enable)
{
	if (enable)
		writeRegister(EN_AA, 0b111111);
	else
		writeRegister(EN_AA, 0);
}

void Nrf24l01::setAutoAck(uint8_t pipe, bool enable)
{
	if (pipe <= 6)
	{
		uint8_t en_aa = readRegister(EN_AA);
		if (enable)
		{
			en_aa |= (1 << pipe);
		}
		else
		{
			en_aa &= ~(1 << pipe);
		}
		writeRegister(EN_AA, en_aa);
	}
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