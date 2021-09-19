#include "driver/spi.h"
#include "driver/gpio.h"
#include "arch.h"
#include <avr/io.h>

signed char SPI::setup()
{
	// configure SS as output to avoid unintened switches to slave mode
	gpio.output(GPIO::pb0, 0);
	// MISO is automatically configured as input
	// Configure SCK and MOSI as output
	DDRB |= _BV(PB2) | _BV(PB1);
	// up to 2 MHz SPI clock
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
}

signed char SPI::xmit(
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i = 0;

	while ((i < tx_len) || (i < rx_len)) {
		if (i < tx_len) {
			SPDR = tx_buf[i];
		} else {
			SPDR = 0;
		}
		while (!(SPSR & _BV(SPIF))) ;
		if (i < rx_len) {
			rx_buf[i] = SPDR;
		}
		i++;
	}
	return 0;
}

SPI spi;
