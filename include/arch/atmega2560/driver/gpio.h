/*
 * Copyright 2021 Daniel Friesel
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
			pa4 = 4,
			pa5 = 5,
			pa6 = 6,
			pa7 = 7,
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
			pc7 = 23,
			pd0 = 24,
			pd1 = 25,
			pd2 = 26,
			pd3 = 27,
			pd4 = 28,
			pd5 = 29,
			pd6 = 30,
			pd7 = 31,
			pe0 = 32,
			pe1 = 33,
			pe2 = 34,
			pe3 = 35,
			pe4 = 36,
			pe5 = 37,
			pe6 = 38,
			pe7 = 39,
			pf0 = 40,
			pf1 = 41,
			pf2 = 42,
			pf3 = 43,
			pf4 = 44,
			pf5 = 45,
			pf6 = 46,
			pf7 = 47,
			pg0 = 48,
			pg1 = 49,
			pg2 = 50,
			pg3 = 51,
			pg4 = 52,
			pg5 = 53,
			ph0 = 56,
			ph1 = 57,
			ph2 = 58,
			ph3 = 59,
			ph4 = 60,
			ph5 = 61,
			ph6 = 62,
			ph7 = 63,
			pj0 = 64,
			pj1 = 65,
			pj2 = 66,
			pj3 = 67,
			pj4 = 68,
			pj5 = 69,
			pj6 = 70,
			pj7 = 71,
			pk0 = 72,
			pk1 = 73,
			pk2 = 74,
			pk3 = 75,
			pk4 = 76,
			pk5 = 77,
			pk6 = 78,
			pk7 = 79,
			pl0 = 80,
			pl1 = 81,
			pl2 = 82,
			pl3 = 83,
			pl4 = 84,
			pl5 = 85,
			pl6 = 86,
			pl7 = 87
		};

		inline void setup() {
			DDRB = _BV(PB7);
		}
		inline volatile uint8_t * pinToPort(uint8_t pin) {
			if (pin <= pa7) {
				return &PORTA;
			}
			if (pin <= pb7) {
				return &PORTB;
			}
			if (pin <= pc7) {
				return &PORTC;
			}
			return &PORTD;
		}
		inline unsigned char pinToBitmask(uint8_t pin) {
			return _BV(pin % 8);
		}
#pragma GCC diagnostic ignored "-Wunused-parameter"
		inline void led_on(unsigned char id) {
			PORTB |= _BV(PB7);
		}
		inline void led_off(unsigned char id) {
			PORTB &= ~_BV(PB7);
		}
		inline void led_toggle(unsigned char id) {
			PINB = _BV(PB7);
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
#pragma GCC diagnostic pop
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
					PORTC |= _BV(pin - 16);
				} else {
					PORTC &= ~_BV(pin - 16);
				}
			} else if (pin < 32) {
				if (value) {
					PORTD |= _BV(pin - 24);
				} else {
					PORTD &= ~_BV(pin - 24);
				}
			}
		}
};

extern GPIO gpio;

#endif
