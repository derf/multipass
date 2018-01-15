#ifndef SOFTI2C_H
#define SOFTI2C_H

class SoftI2C {
	private:
		SoftI2C(const SoftI2C &copy);

		unsigned char sda, scl;

		void start();
		void stop();
		bool tx(unsigned char byte);
		unsigned char rx(bool send_ack);

	public:
		SoftI2C(unsigned char sda, unsigned char scl) : sda(sda), scl(scl) {}
		signed char setup();
		void scan(unsigned int *results);
		signed char xmit(unsigned char address,
				unsigned char tx_len, unsigned char *tx_buf,
				unsigned char rx_len, unsigned char *rx_buf);
};

extern SoftI2C i2c;

#endif
