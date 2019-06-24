#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>
#include "driver/gpio.h"
#include "arch.h"

#define rf24_max(a, b) ((a) > (b) ? (a) : (b))
#define rf24_min(a, b) ((a) < (b) ? (a) : (b))

class Nrf24l01
{
private:
	Nrf24l01(const Nrf24l01 &copy);
	unsigned char txbuf[2];
	unsigned char rxbuf[2];

	bool p_variant;					  /* False for RF24L01 and true for RF24L01P */
	uint8_t payload_size;			  /**< Fixed size of payloads */
	bool dynamic_payloads_enabled;	/**< Whether dynamic payloads are enabled. */
	uint8_t pipe0_reading_address[5]; /**< Last address set on pipe 0 for reading. */
	uint8_t addr_width;				  /**< The address width to use - 3,4 or 5 bytes. */
	uint32_t txRxDelay;				  /**< Var for adjusting delays depending on datarate */

	/**
   * Write a single byte to a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @param value The new value to write
   * @return Current value of status register
   */
	uint8_t writeRegister(uint8_t reg, uint8_t value);

	/**
   * Write a chunk of data to a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @param buf Where to get the data
   * @param len How many bytes of data to transfer
   * @return Current value of status register
   */
	uint8_t writeRegister(uint8_t reg, const uint8_t *buf, uint8_t len);

	/**
   * Read single byte from a register
   *
   * @param reg Which register. Use constants from nRF24L01.h
   * @return Current value of register @p reg
   */
	uint8_t readRegister(uint8_t reg);

	/**
   * Write the transmit payload
   *
   * The size of data written is the fixed payload size, see getPayloadSize()
   *
   * @param buf Where to get the data
   * @param len Number of bytes to be sent
   * @return Current value of status register
   */
	uint8_t writePayload(const void *buf, uint8_t data_len, const uint8_t writeType);

	inline void csnHigh()
	{
		gpio.write(NRF24L01_CS_PIN, 1);
		arch.delay_us(5);
	}
	inline void csnLow()
	{
		gpio.write(NRF24L01_CS_PIN, 0);
		arch.delay_us(5);
	}
	inline void beginTransaction()
	{
		csnLow();
	}
	inline void endTransaction()
	{
		csnHigh();
	}

public:
	Nrf24l01() : payload_size(32), dynamic_payloads_enabled(false), addr_width(5) { pipe0_reading_address[0] = 0; }

	/**
		 * Power Amplifier level.
		 *
		 * For use with setPALevel()
		 */
	enum rf24_pa_dbm_e
	{
		RF24_PA_MIN = 0,
		RF24_PA_LOW,
		RF24_PA_HIGH,
		RF24_PA_MAX,
		RF24_PA_ERROR
	};

	/**
		 * Data rate.  How fast data moves through the air.
		 *
		 * For use with setDataRate()
		 */
	enum rf24_datarate_e
	{
		RF24_1MBPS = 0,
		RF24_2MBPS,
		RF24_250KBPS
	};

	/**
		 * CRC Length.  How big (if any) of a CRC is included.
		 *
		 * For use with setCRCLength()
		 */
	enum rf24_crclength_e
	{
		RF24_CRC_DISABLED = 0,
		RF24_CRC_8,
		RF24_CRC_16
	};

	/**
   * Enter low-power mode
   *
   * To return to normal power mode, call powerUp().
   *
   * @note After calling startListening(), a basic radio will consume about 13.5mA
   * at max PA level.
   * During active transmission, the radio will consume about 11.5mA, but this will
   * be reduced to 26uA (.026mA) between sending.
   * In full powerDown mode, the radio will consume approximately 900nA (.0009mA)
   *
   * @code
   * radio.powerDown();
   * avr_enter_sleep_mode(); // Custom function to sleep the device
   * radio.powerUp();
   * @endcode
   */
	void powerDown(void);

	/**
   * Leave low-power mode - required for normal radio operation after calling powerDown()
   *
   * To return to low power mode, call powerDown().
   * @note This will take up to 5ms for maximum compatibility
   */
	void powerUp(void);

	/**
   * Empty the transmit buffer. This is generally not required in standard operation.
   * May be required in specific cases after stopListening() , if operating at 250KBPS data rate.
   *
   * @return Current value of status register
   */
	uint8_t flushTx(void);

	/**
   * Empty the receive buffer
   *
   * @return Current value of status register
   */
	uint8_t flushRx(void);

	void setup();
	/**
   * Set Power Amplifier (PA) level to one of four levels:
   * RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
   *
   * The power levels correspond to the following output levels respectively:
   * NRF24L01: -18dBm, -12dBm,-6dBM, and 0dBm
   *
   * SI24R1: -6dBm, 0dBm, 3dBM, and 7dBm.
   *
   * @param level Desired PA level.
   */
	void setPALevel(uint8_t level); // 0 (-18B), 1 (-12dB), 2 (-6dB), 3 (0dB)

	/**
  * Set the address width from 3 to 5 bytes (24, 32 or 40 bit)
  *
  * @param a_width The address width to use: 3,4 or 5
  */

	void setAddressWidth(uint8_t a_width);

	/**
   * Set the number and delay of retries upon failed submit
   *
   * @param delay How long to wait between each retry, in multiples of 250us,
   * max is 15.  0 means 250us, 15 means 4000us.
   * @param count How many retries before giving up, max 15
   */
	void setRetries(uint8_t delay, uint8_t count);

	/**
   * Set RF communication channel
   *
   * @param channel Which RF channel to communicate on, 0-125
   */
	void setChannel(uint8_t channel);

	/**
   * Get RF communication channel
   *
   * @return The currently configured RF Channel
   */
	uint8_t getChannel(void);

	/**
   * Set Static Payload Size
   *
   * This implementation uses a pre-stablished fixed payload size for all
   * transmissions.  If this method is never called, the driver will always
   * transmit the maximum payload size (32 bytes), no matter how much
   * was sent to write().
   *
   * @todo Implement variable-sized payloads feature
   *
   * @param size The number of bytes in the payload
   */
	void setPayloadSize(uint8_t size);

	/**
   * Get Static Payload Size
   *
   * @see setPayloadSize()
   *
   * @return The number of bytes in the payload
   */
	uint8_t getPayloadSize(void);

	/**
   * Set the transmission data rate
   *
   * @warning setting RF24_250KBPS will fail for non-plus units
   *
   * @param speed RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
   * @return true if the change was successful
   */
	bool setDataRate(rf24_datarate_e speed);

	/**
  * The radio will generate interrupt signals when a transmission is complete,
  * a transmission fails, or a payload is received. This allows users to mask
  * those interrupts to prevent them from generating a signal on the interrupt
  * pin. Interrupts are enabled on the radio chip by default.
  *
  * @code
  * 	Mask all interrupts except the receive interrupt:
  *
  *		radio.maskIRQ(1,1,0);
  * @endcode
  *
  * @param tx_ok  Mask transmission complete interrupts
  * @param tx_fail  Mask transmit failure interrupts
  * @param rx_ready Mask payload received interrupts
  */
	void maskIRQ(bool tx_ok, bool tx_fail, bool rx_ready);

	/**
   * Turn on or off the special features of the chip
   *
   * The chip has certain 'features' which are only available when the 'features'
   * are enabled.  See the datasheet for details.
   */
	void toggleFeatures(void);
	/**
   * Be sure to call openWritingPipe() first to set the destination
   * of where to write to.
   *
   * This blocks until the message is successfully acknowledged by
   * the receiver or the timeout/retransmit maxima are reached.  In
   * the current configuration, the max delay here is 60-70ms.
   *
   * The maximum size of data written is the fixed payload size, see
   * getPayloadSize().  However, you can write less, and the remainder
   * will just be filled with zeroes.
   *
   * TX/RX/RT interrupt flags will be cleared every time write is called
   *
   * @param buf Pointer to the data to be sent
   * @param len Number of bytes to be sent
   *
   * @code
   * radio.stopListening();
   * radio.write(&data,sizeof(data));
   * @endcode
   * @return True if the payload was delivered successfully false if not
   */
	uint8_t write(const void *buf, uint8_t len, bool blocking);

	/**
   * Start listening on the pipes opened for reading.
   *
   * 1. Be sure to call openReadingPipe() first.
   * 2. Do not call write() while in this mode, without first calling stopListening().
   * 3. Call available() to check for incoming traffic, and read() to get it.
   *
   * @code
   * Open reading pipe 1 using address CCCECCCECC
   *
   * byte address[] = { 0xCC,0xCE,0xCC,0xCE,0xCC };
   * radio.openReadingPipe(1,address);
   * radio.startListening();
   * @endcode
   */
	void startListening(void);

	/**
   * Stop listening for incoming messages, and switch to transmit mode.
   *
   * Do this before calling write().
   * @code
   * radio.stopListening();
   * radio.write(&data,sizeof(data));
   * @endcode
   */
	void stopListening(void);

	/**
   * Check whether there are bytes available to be read
   * @code
   * if(radio.available()){
   *   radio.read(&data,sizeof(data));
   * }
   * @endcode
   * @return True if there is a payload available, false if none is
   */
	bool available(void);

	/**
   * Read the available payload
   *
   * The size of data read is the fixed payload size, see getPayloadSize()
   *
   * @note I specifically chose 'void*' as a data type to make it easier
   * for beginners to use.  No casting needed.
   *
   * @note No longer boolean. Use available to determine if packets are
   * available. Interrupt flags are now cleared during reads instead of
   * when calling available().
   *
   * @param buf Pointer to a buffer where the data should be written
   * @param len Maximum number of bytes to read into the buffer
   *
   * @code
   * if(radio.available()){
   *   radio.read(&data,sizeof(data));
   * }
   * @endcode
   * @return No return value. Use available().
   */
	void read(void *buf, uint8_t len);

	/**
   * Open a pipe for reading
   *
   * Up to 6 pipes can be open for reading at once.  Open all the required
   * reading pipes, and then call startListening().
   *
   * @see openWritingPipe
   * @see setAddressWidth
   *
   * @note Pipes 0 and 1 will store a full 5-byte address. Pipes 2-5 will technically
   * only store a single byte, borrowing up to 4 additional bytes from pipe #1 per the
   * assigned address width.
   * @warning Pipes 1-5 should share the same address, except the first byte.
   * Only the first byte in the array should be unique, e.g.
   * @code
   *   uint8_t addresses[][6] = {"1Node","2Node"};
   *   openReadingPipe(1,addresses[0]);
   *   openReadingPipe(2,addresses[1]);
   * @endcode
   *
   * @warning Pipe 0 is also used by the writing pipe.  So if you open
   * pipe 0 for reading, and then startListening(), it will overwrite the
   * writing pipe.  Ergo, do an openWritingPipe() again before write().
   *
   * @param number Which pipe# to open, 0-5.
   * @param address The 24, 32 or 40 bit address of the pipe to open.
   */
	void openReadingPipe(uint8_t number, const uint8_t *address);

	/**
   * Close a pipe after it has been previously opened.
   * Can be safely called without having previously opened a pipe.
   * @param pipe Which pipe # to close, 0-5.
   */
	void closeReadingPipe(uint8_t pipe);

	uint8_t getStatus();
};

extern Nrf24l01 nrf24l01;

#endif
