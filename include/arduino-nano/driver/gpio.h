#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			pb0 = 8,
			pb1 = 9,
			pb2 = 10,
			pb3 = 11,
			pb4 = 12,
			pb5 = 13,
			pb6 = 14,
			pb7 = 15,
			pc0 = 16,
			pc1 = 17,
			pc2 = 18,
			pc3 = 19,
			pc4 = 20,
			pc5 = 21,
			pc6 = 22,
			pd0 = 24,
			pd1 = 25,
			pd2 = 26,
			pd3 = 27,
			pd4 = 28,
			pd5 = 29,
			pd6 = 30,
			pd7 = 31
		};

		void setup();
		void led_on(unsigned char id);
		void led_off(unsigned char id);
		void led_toggle(unsigned char id);
		inline void input(unsigned char const pin) {
			if (pin < 8) {
			} else if (pin < 16) {
				DDRB &= ~_BV(pin - 8);
			} else if (pin < 24) {
				DDRC &= ~_BV(pin - 16);
			} else if (pin < 32) {
				DDRD &= ~_BV(pin - 24);
			}
		}
		inline void output(unsigned char const pin) {
			if (pin < 8) {
			} else if (pin < 16) {
				DDRB |= _BV(pin - 8);
			} else if (pin < 24) {
				DDRC |= _BV(pin - 16);
			} else if (pin < 32) {
				DDRD |= _BV(pin - 24);
			}
		}
		inline unsigned char read(unsigned char const pin) {
			if (pin < 8) {
			}
			if (pin < 16) {
				return (PINB & _BV(pin - 8));
			}
			if (pin < 24) {
				return (PINC & _BV(pin - 16));
			}
			if (pin < 32) {
				return (PIND & _BV(pin - 24));
			}
			return 0;
		}
		inline void write(unsigned char const pin, unsigned char value) {
			if (pin < 8) {
			} else if (pin < 16) {
				if (value) {
					PORTB |= _BV(pin - 8);
				} else {
					PORTB &= ~_BV(pin - 8);
				}
			} else if (pin < 24) {
				if (value) {
					PORTB |= _BV(pin - 16);
				} else {
					PORTB &= ~_BV(pin - 16);
				}
			} else if (pin < 32) {
				if (value) {
					PORTB |= _BV(pin - 24);
				} else {
					PORTB &= ~_BV(pin - 24);
				}
			}
		}
};

extern GPIO gpio;

#endif
