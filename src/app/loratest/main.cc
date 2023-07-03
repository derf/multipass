/*
 * Copyright 2020 Birte Kristina Friesel
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
		unsigned char write_pos, read_pos;

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

void loop(void)
{
	static unsigned int i = 0;
	gpio.led_off(0);
	// does not work on MSP430 3V3
	// works when connected to MSP430 5V0, AUX should be disconnected in that case
	// RX seems to work best without intermittent TX
	sout << "Hello! My value is " << i << endl;
	if (sin.hasKey()) {
		gpio.led_on(0);
		kout << "RX: ";
		while (sin.hasKey()) {
			kout << sin.getKey();
		}
		kout << endl;
	}
	i++;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	sout.setup();
	sin.setup();

	gpio.led_on(1);

	// AUX
	gpio.input(GPIO::p4_2);

	// set module to sleep mode

	// M0
	gpio.output(GPIO::p2_4, 1);

	// M1
	gpio.output(GPIO::p4_3, 1);

	arch.delay_ms(100);
	sout << "\xc2\x00\x00\x1a\x17\x47";
	arch.delay_ms(50);
	gpio.write(GPIO::p2_4, 0);
	gpio.write(GPIO::p4_3, 0);
	arch.delay_ms(10);

	gpio.led_off(1);
	gpio.led_on(0);

	arch.idle_loop();

	return 0;
}
