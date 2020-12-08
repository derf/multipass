/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

class GPIO {
	private:
		GPIO(const GPIO &copy);

	public:
		GPIO () {}

		enum Pin : unsigned char {
			pa0 = 0,
			pa1 = 1,
			pa2 = 2,
			pa3 = 3,
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
			pc7 = 23,
			pd0 = 24,
			pd1 = 25,
			pd2 = 26,
			pd3 = 27,
			pd4 = 28,
			pd5 = 29,
			pd6 = 30,
			pd7 = 31
		};

		inline void setup() {
			PORTB = 0;
			DDRB = 0;
			PORTD = 0x7f;
			DDRD = 0xff;
		}
		inline void led_on(unsigned char id) {
			PORTB |= _BV(id);
		}
		inline void led_off(unsigned char id) {
			PORTB &= ~_BV(id);
		}
		inline void led_toggle(unsigned char id) {
			PORTB ^= _BV(id);
		}
		inline void input(unsigned char const pin) {
			if (pin < 8) {
				DDRA &= ~_BV(pin);
			} else if (pin < 16) {
				DDRB &= ~_BV(pin - 8);
			} else if (pin < 24) {
				DDRC &= ~_BV(pin - 16);
			} else if (pin < 32) {
				DDRD &= ~_BV(pin - 24);
			}
		}
		inline void input(unsigned char const pin, unsigned char const pull) {
			if (pin < 8) {
				DDRA &= ~_BV(pin);
				PORTA |= _BV(pin);
			} else if (pin < 16) {
				DDRB &= ~_BV(pin - 8);
				PORTB |= _BV(pin - 8);
			} else if (pin < 24) {
				DDRC &= ~_BV(pin - 16);
				PORTC |= _BV(pin - 16);
			} else if (pin < 32) {
				DDRD &= ~_BV(pin - 24);
				PORTD |= _BV(pin - 24);
			}
		}
		inline void output(unsigned char const pin) {
			if (pin < 8) {
				DDRA |= _BV(pin);
			} else if (pin < 16) {
				DDRB |= _BV(pin - 8);
			} else if (pin < 24) {
				DDRC |= _BV(pin - 16);
			} else if (pin < 32) {
				DDRD |= _BV(pin - 24);
			}
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			if (pin < 8) {
				PORTA = value ? (PORTA | _BV(pin)) : (PORTA & ~_BV(pin));
				DDRA |= _BV(pin);
			} else if (pin < 16) {
				PORTB = value ? (PORTB | _BV(pin - 8)) : (PORTB & ~_BV(pin - 8));
				DDRB |= _BV(pin - 8);
			} else if (pin < 24) {
				PORTC = value ? (PORTC | _BV(pin - 16)) : (PORTC & ~_BV(pin - 16));
				DDRC |= _BV(pin - 16);
			} else if (pin < 32) {
				PORTD = value ? (PORTD | _BV(pin - 24)) : (PORTD & ~_BV(pin - 24));
				DDRD |= _BV(pin - 24);
			}
		}
		inline unsigned char read(unsigned char const pin) {
			if (pin < 8) {
				return (PINA & _BV(pin));
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
				if (value) {
					PORTA |= _BV(pin);
				} else {
					PORTA &= ~_BV(pin);
				}
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
		/*
		inline void enable_int(unsigned char const pin) {
			if (pin < 8) {
			} else if (pin < 16) {
				PCMSK0 |= _BV(pin - 8);
			} else if (pin < 24) {
				PCMSK1 |= _BV(pin - 16);
			} else if (pin < 32) {
				PCMSK2 |= _BV(pin - 24);
			}
		}
		inline void disable_int(unsigned char const pin) {
			if (pin < 8) {
			} else if (pin < 16) {
				PCMSK0 &= ~_BV(pin - 8);
			} else if (pin < 24) {
				PCMSK1 &= ~_BV(pin - 16);
			} else if (pin < 32) {
				PCMSK2 &= ~_BV(pin - 24);
			}
		}
		*/
};

extern GPIO gpio;

#endif
