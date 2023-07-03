/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

class SerialOutput : public OutputStream {
	private:
		SerialOutput(const SerialOutput &copy);

	public:
		SerialOutput () {}
		void setup();

		virtual void put(char c) override;
};

/*
 * Baud rate calculation according to datasheet:
 * N := f_{BRCLK} / Baudrate = F_CPU / 115200 in our case
 * if N <= 16: OS16 = 0, UCBR0 = int(N)
 * if N > 16: OS16 = 1, UCBR0 = int(N/16), UCBRF0 = int(((n/16) - int(n/16)) * 16) = int(N)%16
 * Set UCBRS0 according to table 21-4
 */

void SerialOutput::setup()
{
	UCA1CTLW0 |= UCSWRST;
#if F_CPU == 16000000UL
	// 16M / 9600 == 1666.6667 -> UCOS16 = 1, UCBR0 = 104, UCBRF0 = 2, UCBRS0 = 0xd6 ("0.6667")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = UCOS16 | (2<<4) | 0xd600;
	UCA1BR0 = 104;
#elif F_CPU == 8000000UL
	// 8M / 9600 == 833.3333 -> UCOS16 = 1, UCBR0 = 52, UCBRF0 = 1, UCBRS0 = 0x49 ("0.3335")
	UCA1CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA1MCTLW = UCOS16 | (1<<4) | 0x4900;
	UCA1BR0 = 52;
#else
#error Unsupported F_CPU
#endif

	UCA1IRCTL = 0;
	UCA1ABCTL = 0;

	P2REN &= ~(BIT5 | BIT6);
	P2SEL0 &= ~(BIT5 | BIT6);
	P2SEL1 |= BIT5 | BIT6;

	UCA1CTLW0 &= ~UCSWRST;

	//UCA1IE |= UCRXIE;
}

void SerialOutput::put(char c)
{
	while (!(UCA1IFG & UCTXIFG));
	UCA1TXBUF = c;

	if (c == '\n') {
		put('\r');
	}
}

SerialOutput sout;

class SerialInput {
	private:
		SerialInput(const SerialInput &copy);
		char buffer[64];
		volatile unsigned char write_pos, read_pos;

	public:
		SerialInput() : write_pos(0), read_pos(0) {}
		void setup();
		bool hasKey();
		char getKey();

		inline void addKey(char key) {
			buffer[write_pos++] = key;
			write_pos %= 64;
		}
};

void SerialInput::setup()
{
	UCA1IE |= UCRXIE;
}

bool SerialInput::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char SerialInput::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= 64;
	return ret;
}

SerialInput sin;

__attribute__((interrupt(USCI_A1_VECTOR))) __attribute__((wakeup)) void handle_stdin()
{
	if (UCA1IFG & UCRXIFG) {
		sin.addKey(UCA1RXBUF);
	}
}

const unsigned char mhz19c_read[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

void loop(void)
{
	static unsigned int interval = 0;
	static char buf[9];
	unsigned char i;
	unsigned int co2_ppm;
	signed int temperature;
	unsigned int unknown_status;
	unsigned int unknown_cal;
	if (interval++ == 2) {
		gpio.led_on(0);
		for (i = 0; i < sizeof(mhz19c_read); i++) {
			sout.put(mhz19c_read[i]);
		}
		arch.sleep_ms(100);
		buf[1] = 0;
		for (i = 0; i < 9; i++) {
			if (sin.hasKey()) {
				buf[i] = sin.getKey();
			} else {
				break;
			}
		}
		if (i == 9) {
			if (buf[0] == 0xff && buf[1] == 0x86) {
				// see https://revspace.nl/MHZ19#Command_0x86_.28read_concentration.29
				co2_ppm = (buf[2] << 8) + buf[3];
				temperature = buf[4] - 40;
				unknown_status = buf[5];
				unknown_cal = (buf[6] << 8) + buf[7];
				kout << "CO₂: " << co2_ppm << " ppm @ " << temperature << " °c     ";
				kout << unknown_status << " / " << unknown_cal << endl;
			} else {
				kout << "! communication error, expected 0xff 0x86, got" << hex << (int)buf[0] << ' ' << (int)buf[1] << dec << endl;
			}
		} else {
			kout << "! timeout" << endl;
		}
		gpio.led_off(0);
		interval = 0;
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	sout.setup();
	sin.setup();

	arch.idle_loop();

	return 0;
}
