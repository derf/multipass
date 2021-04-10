/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/soft_i2c.h"
#include "driver/gpio.h"
#include "arch.h"

#ifndef F_I2C
#define F_I2C 100000
#endif

#ifdef SOFTI2C_TIMER
#include "driver/timer.h"
volatile unsigned char timer_done = 0;
#endif

#ifdef SOFTI2C_PULLUP_INTERNAL
#define SDA_HIGH gpio.input(sda, 1)
#define SDA_LOW gpio.output(sda, 0)
#define SCL_HIGH gpio.input(scl, 1)
#define SCL_LOW gpio.output(scl, 0)
#elif SOFTI2C_PULLUP_EXTERNAL
#define SDA_HIGH { gpio.input(sda); gpio.write(sda_pull, 1); }
#define SDA_LOW { gpio.write(sda_pull, 0); gpio.output(sda); }
#define SCL_HIGH { gpio.input(scl); gpio.write(scl_pull, 1); }
#define SCL_LOW { gpio.write(scl_pull, 0); gpio.output(scl); }
#else /* !SOFTI2C_PULLUP_{INTERNAL,EXTERNAL} */
#define SDA_HIGH gpio.input(sda)
#define SDA_LOW gpio.output(sda)
#define SCL_HIGH gpio.input(scl)
#define SCL_LOW gpio.output(scl)
#endif /* SOFTI2C_PULLUP */


inline void i2c_wait()
{
#ifdef SOFTI2C_TIMER
/*
 * Note: On MSP430, if F_CPU / F_I2C < 60 (approx.), i2c_wait() does not
 * work.  Probably related to missed interrupts / not enough cycles between
 * start and idle?  We work around this for now by simulating an immediate
 * return in these cases.
 */
#if MULTIPASS_ARCH_msp430fr5969lp && ((F_CPU / F_I2C) < 60)
#else
	timer_done = 0;
	timer.start(1);
	while (!timer_done) {
		arch.idle();
	}
	timer.stop();
#endif
#else /* !SOFTI2C_TIMER */
#if (500000UL / F_I2C) > 11
	arch.delay_us((500000UL / F_I2C) - 10);
#else
	arch.delay_us(500000UL / F_I2C);
#endif
#endif /* SOFTI2C_TIMER */
}

signed char SoftI2C::setup()
{
#ifdef SOFTI2C_PULLUP_EXTERNAL
	gpio.output(sda_pull);
	gpio.output(scl_pull);
#endif
#ifdef SOFTI2C_PULLUP_FIXED_GPIO
#if MULTIPASS_ARCH_msp430fr5969lp
	gpio.output(GPIO::p1_4, 1);
	gpio.output(GPIO::p1_5, 1);
#elif MULTIPASS_ARCH_msp430fr5994lp
	gpio.output(GPIO::p8_2, 1);
	gpio.output(GPIO::p8_3, 1);
#else
#error "softi2c_pullup=gpio not supported on this architecture"
#endif /* MULTIPASS_ARCH_* */
#endif /* SOFTI2C_PULLUP_FIXED_GPIO */
	SDA_HIGH;
	SCL_HIGH;
#ifdef SOFTI2C_TIMER
	/*
	 * I2C frequency is the time between two SCL low->high transitions
	 * (or high->low, whatever you prefer). For the timer, we need to set the
	 * time between SCL low->high and the following high->low transition
	 * (and vice versa), which is twice the desired I2C frequency. Also,
	 * timer.setup wants kHz and not Hz, so we have
	 * Timer Freq [kHz] = I2C Freq [Hz] * 2 / 1000
	 */
	timer.setup_khz(F_I2C / 500);
#endif
	return 0;
}

void SoftI2C::start()
{
	SDA_HIGH;
	SCL_HIGH;
	i2c_wait(); // t_{SU,STA} >= 4.7 µs (für Sr)
	SDA_LOW;
	i2c_wait(); // t_{HD,STA} >= 4 µs
	SCL_LOW;
	i2c_wait();
}

void SoftI2C::stop()
{
	SCL_LOW;
	SDA_LOW;
	i2c_wait();
	SCL_HIGH;
	i2c_wait(); // t_{SU,STO} >= 4 µs
	SDA_HIGH;
	i2c_wait(); // t_{BUF} >= 4.7 µs
}

bool SoftI2C::tx(unsigned char byte)
{
	unsigned char got_ack = 0;
	for (unsigned char i = 0; i <= 8; i++) {
		if ((byte & 0x80) || (i == 8)) {
			SDA_HIGH;
		} else {
			SDA_LOW;
		}
		byte <<= 1;
		i2c_wait();
		SCL_HIGH;
		i2c_wait();

		// Avoid hanging indefinitely if the bus / device is stuck
		for (unsigned char i = 0; i < 200 && !gpio.read(scl); i++) {
			i2c_wait();
		}
		if (!gpio.read(scl)) {
			return false;
		}

		if (i == 8) {
			if (!gpio.read(sda)) {
				got_ack = 1;
			}
		}
		SCL_LOW;
	}
	return got_ack;
}

unsigned char SoftI2C::rx(bool send_ack)
{
	unsigned char byte = 0;
	SDA_HIGH;
	for (unsigned char i = 0; i <= 8; i++) {
		i2c_wait();
		SCL_HIGH;
		i2c_wait();

		// Avoid hanging indefinitely if the bus / device is stuck
		for (unsigned char i = 0; i < 200 && !gpio.read(scl); i++) {
			i2c_wait();
		}
		if (!gpio.read(scl)) {
			return false;
		}

		if ((i < 8) && gpio.read(sda)) {
			byte |= 1 << (7 - i);
		}
		SCL_LOW;
		if ((i == 7) && send_ack) {
			SDA_LOW;
		} else if ((i == 8) && send_ack) {
			SDA_HIGH;
		}
	}
	return byte;
}

void SoftI2C::scan(unsigned int *results)
{
	unsigned char i2caddr;
	for (unsigned char address = 0; address < 128; address++) {

		i2caddr = (address << 1) | 0;

		start();

		if (tx(i2caddr)) {
			results[address / (8 * sizeof(unsigned int))] |= 1 << (address % (8 * sizeof(unsigned int)));
			stop();
		}
	}
	stop();
}

signed char SoftI2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	unsigned char i;

	if (tx_len) {
		start();
		if (tx((address << 1) | 0) == false) {
			return -1;
		}

		for (i = 0; i < tx_len; i++) {
			if (tx(tx_buf[i]) == false) {
				return -1;
			}
		}
	}
	if (rx_len) {
		start();
		if (tx((address << 1) | 1) == false) {
			return -1;
		}

		for (i = 1; i <= rx_len; i++) {
			rx_buf[i-1] = rx((i < rx_len) * 1);
		}
	}

	stop();

	return 0;
}

#ifdef SOFTI2C_TIMER
ON_TIMER_INTERRUPT_head
	timer_done = 1;
ON_TIMER_INTERRUPT_tail
#endif

#ifndef DRIVER_HARDWARE_I2C

#if SOFTI2C_PULLUP_EXTERNAL
#ifdef MULTIPASS_ARCH_msp430fr5969lp
SoftI2C i2c(GPIO::p1_6, GPIO::p1_7, GPIO::p1_4, GPIO::p1_5);
#elif MULTIPASS_ARCH_msp430fr5994lp
SoftI2C i2c(GPIO::p5_0, GPIO::p5_1, GPIO::p8_2, GPIO::p8_3);
#else
#error "softi2c_pullup = external not supported on this architecture"
#endif /* MULTIPASS_ARCH_* */
#else
#ifdef MULTIPASS_ARCH_esp8266
SoftI2C i2c(GPIO::d6, GPIO::d7);
#elif MULTIPASS_ARCH_arduino_nano
SoftI2C i2c(GPIO::pc4, GPIO::pc5);
#elif MULTIPASS_ARCH_blinkenrocket
SoftI2C i2c(GPIO::pc4, GPIO::pc5);
#elif MULTIPASS_ARCH_msp430fr5969lp
SoftI2C i2c(GPIO::p1_6, GPIO::p1_7);
#elif MULTIPASS_ARCH_msp430fr5994lp
SoftI2C i2c(GPIO::p5_0, GPIO::p5_1);
#elif MULTIPASS_ARCH_posix
SoftI2C i2c(GPIO::px00, GPIO::px01);
#endif /* MULTIPASS_ARCH_* */
#endif /* !SOFTI2C_PULLUP_EXTERNAL */

#endif /* !DRIVER_HARDWARE_I2C */
