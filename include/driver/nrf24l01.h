#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>
#include "driver/gpio.h"
#include "arch.h"

class Nrf24l01 {
	private:
		Nrf24l01(const Nrf24l01 &copy);
		unsigned char txbuf[2];
		unsigned char rxbuf[2];

		uint8_t writeRegister(uint8_t reg, uint8_t value);
		uint8_t readRegister(uint8_t reg);

		inline void csnHigh() {
			gpio.write(NRF24L01_CS_PIN, 1);
			arch.delay_us(5);
		}
		inline void csnLow() {
			gpio.write(NRF24L01_CS_PIN, 0);
			arch.delay_us(5);
		}
		inline void beginTransaction() {
			csnLow();
		}
		inline void endTransaction() {
			csnHigh();
		}

	public:
		Nrf24l01() {}

		/**
		 * Power Amplifier level.
		 *
		 * For use with setPALevel()
		 */
		enum rf24_pa_dbm_e { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR };

		/**
		 * Data rate.  How fast data moves through the air.
		 *
		 * For use with setDataRate()
		 */
		enum rf24_datarate_e { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS };

		/**
		 * CRC Length.  How big (if any) of a CRC is included.
		 *
		 * For use with setCRCLength()
		 */
		enum rf24_crclength_e { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 };


		void setup();
		void powerOn();
		void powerOff();

		void setRetries(uint8_t delay, uint8_t count);
		void setPALevel ( uint8_t level ); // 0 (-18B), 1 (-12dB), 2 (-6dB), 3 (0dB)

		uint8_t getStatus();
};

extern Nrf24l01 nrf24l01;

#endif
