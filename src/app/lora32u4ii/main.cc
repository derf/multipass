/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#include "driver/adc.h"
#include "driver/spi.h"

#define CFG_eu868 1
#define CFG_sx1276_radio 1

#include "radio.h"
#include "oslmic.h"

unsigned char txbuf[4];
unsigned char rxbuf[4];

// DIYMall BSFrance LoRa32u4 II v1.3
// See also: arduino packages/adafruit/hardware/avr/1.4.13/variants/feather32u4/pins_arduino.h
// PB0 -> Charger enable and Vbat/2 measurement voltage divider enable
// PB0 is also the !SS pin, so it must be configured as output in order to use SPI.
// PB5 "D9" <- Vbat/2 via voltage divider. Appears to also have a connection to the user LED
// PC7 "D13" -> User LED
const GPIO::Pin rstpin = GPIO::pd4; // "D4" -> RST
const GPIO::Pin  cspin = GPIO::pb4; // "D8" -> NSS
const GPIO::Pin dio0pin = GPIO::pe6; // "D7" <- DIO0 / IRQ
const GPIO::Pin dio1pin = GPIO::pc6; // "D5" <- DIO1

static void writeReg (u1_t addr, u1_t data) {
	txbuf[0] = addr | 0x80;
	txbuf[1] = data;
	gpio.write(cspin, 0);
	spi.xmit(2, txbuf, 0, rxbuf);
	gpio.write(cspin, 1);
}

static u1_t readReg (u1_t addr) {
	txbuf[0] = addr & 0x7f;
	gpio.write(cspin, 0);
	spi.xmit(1, txbuf, 2, rxbuf);
	gpio.write(cspin, 1);
	return rxbuf[1];
}
/*
static void writeBuf (u1_t addr, xref2u1_t buf, u1_t len) {
	txbuf[0] = addr | 0x80;
	for (uint8_t i = 0; i < len; i++) {
		txbuf[i+1] = buf[i];
	}
	spi.xmit(len+1, txbuf, 0, rxbuf);
}

static void readBuf (u1_t addr, xref2u1_t buf, u1_t len) {
	txbuf[0] = addr & 0x7f;
	spi.xmit(1, txbuf, len, buf);
}
*/

static void writeOpmode(u1_t mode) {
	u1_t const maskedMode = mode & OPMODE_MASK;
	writeReg(RegOpMode, mode);
}

static void opmode (u1_t mode) {
	writeOpmode((readReg(RegOpMode) & ~OPMODE_MASK) | mode);
}



bool radio_init()
{
	// requestModuleActive(1); not required for sx yadayada
	gpio.output(cspin, 1);
#ifdef CFG_sx1276_radio
	gpio.output(rstpin, 0);
#else
	gpio.output(rstpin, 1);
#endif
	arch.delay_ms(1);
	gpio.input(rstpin);
	gpio.write(rstpin, 0); // disable pull-up
	arch.delay_ms(5);
	opmode(OPMODE_SLEEP);

	u1_t v = readReg(RegVersion);
#ifdef CFG_sx1276_radio
	if(v != 0x12 ) {
		kout << "Radio version mismatch: expected " << hex << 0x12 << ", got " << v << endl;
		return false;
	}
#elif CFG_sx1272_radio
	if(v != 0x22) {
		kout << "Radio version mismatch: expected " << hex << 0x22 << ", got " << v << endl;
		return false;
	}
#else
#error Missing CFG_sx1272_radio/CFG_sx1276_radio
#endif
	return true;
}

void loop(void)
{
	//gpio.led_toggle(1);
#ifdef TIMER_S
	kout << dec << uptime.get_s() << endl;
#else
	kout << "beep boop" << endl;
#endif
	kout << "VCC  = " << adc.getVCC_mV() << " mV" << endl;
	kout << "Vbat = " << adc.getVBat_mV(false) << " mV" << endl;
	gpio.led_toggle(0);
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	spi.setup();
	gpio.input(GPIO::pb5);

	radio_init();

	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;

	arch.idle_loop();

	return 0;
}
