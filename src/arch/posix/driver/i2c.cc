/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/i2c.h"
#include "arch.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>


signed char I2C::setup()
{
	return 0;
}

signed char I2C::xmit(unsigned char address,
		unsigned char tx_len, unsigned char *tx_buf,
		unsigned char rx_len, unsigned char *rx_buf)
{
	struct i2c_msg messages[2];
	struct i2c_rdwr_ioctl_data transaction;
	transaction.nmsgs = 0;
	transaction.msgs = messages;

	if (tx_len) {
		messages[transaction.nmsgs].addr = address;
		messages[transaction.nmsgs].flags = 0;
		messages[transaction.nmsgs].len = tx_len;
		messages[transaction.nmsgs].buf = tx_buf;
		transaction.nmsgs++;
	}
	if (rx_len) {
		messages[transaction.nmsgs].addr = address;
		messages[transaction.nmsgs].flags = I2C_M_RD;
		messages[transaction.nmsgs].len = rx_len;
		messages[transaction.nmsgs].buf = rx_buf;
		transaction.nmsgs++;
	}

	int fh = open(i2cbus, O_RDWR);

	if (fh < 0) {
		return -1;
	}

	if (ioctl(fh, I2C_RDWR, &transaction) == -1) {
		return -1;
	}

	close(fh);
	return 0;
}

I2C i2c("/dev/i2c-1");
