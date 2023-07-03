/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef SOFTI2C_H
#define SOFTI2C_H

class SoftI2C {
	private:
		SoftI2C(const SoftI2C &copy);

		unsigned char sda, scl;
#if CONFIG_driver_softi2c_pullup_dynamic_external
		unsigned char sda_pull, scl_pull;
#endif

		void start();
		void stop();
		bool tx(unsigned char byte);
		unsigned char rx(bool send_ack);

	public:
#if CONFIG_driver_softi2c_pullup_dynamic_external
		SoftI2C(unsigned char sda, unsigned char scl,
				unsigned char sda_pull, unsigned char scl_pull) :
			sda(sda), scl(scl), sda_pull(sda_pull), scl_pull(scl_pull) {}
#else
		SoftI2C(unsigned char sda, unsigned char scl) : sda(sda), scl(scl) {}
#endif
		signed char setup();
		void scan(unsigned int *results);
		signed char xmit(unsigned char address,
				unsigned char tx_len, unsigned char *tx_buf,
				unsigned char rx_len, unsigned char *rx_buf);
};

#ifndef DRIVER_HARDWARE_I2C
extern SoftI2C i2c;
#endif

#endif
