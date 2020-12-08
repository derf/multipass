/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/spi.h"
#include "driver/gpio.h"
#include "arch.h"
#include <avr/io.h>

signed char SPI::setup()
{
	// configure SS as output to avoid unintened switches to slave mode
	gpio.output(GPIO::pb2, 0);
	// up to 1 MHz SPI clock
#if F_CPU == 16000000UL
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
#elif F_CPU == 8000000UL
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
#else
	SPCR = _BV(SPE) | _BV(MSTR);
#endif
}

signed char SPI::xmit(
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;

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
