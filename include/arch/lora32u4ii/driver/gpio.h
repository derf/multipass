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
			pe2 = 34,
			pe6 = 38,
			pf0 = 40,
			pf1 = 41,
			pf4 = 44,
			pf5 = 45,
			pf6 = 46,
			pf7 = 47
		};

		inline void setup() {
			DDRB = _BV(PB5);
		}
		inline volatile uint8_t * pinToPort(uint8_t pin) {
			if (pin <= pb7) {
				return &PORTB;
			}
			if (pin <= pc6) {
				return &PORTC;
			}
			return &PORTD;
		}
		inline unsigned char pinToBitmask(uint8_t pin) {
			return _BV(pin % 8);
		}
#pragma GCC diagnostic ignored "-Wunused-parameter"
		inline void led_on(unsigned char id) {
			PORTC |= _BV(PC7);
		}
		inline void led_off(unsigned char id) {
			PORTC &= ~_BV(PC7);
		}
		inline void led_toggle(unsigned char id) {
			PINC = _BV(PC7);
		}
		inline void input(unsigned char const pin) {
			if (pin < 8) {
			} else if (pin < 16) {
				DDRB &= ~_BV(pin - 8);
			} else if (pin < 24) {
				DDRC &= ~_BV(pin - 16);
			} else if (pin < 32) {
				DDRD &= ~_BV(pin - 24);
			} else if (pin < 40) {
				DDRE &= ~_BV(pin - 32);
			} else if (pin < 48) {
				DDRF &= ~_BV(pin - 40);
			}
		}
#pragma GCC diagnostic pop
		inline void input(unsigned char const pin, unsigned char const pull) {
			if (pin < 8) {
			} else if (pin < 16) {
				DDRB &= ~_BV(pin - 8);
				PORTB |= _BV(pin - 8);
			} else if (pin < 24) {
				DDRC &= ~_BV(pin - 16);
				PORTC |= _BV(pin - 16);
			} else if (pin < 32) {
				DDRD &= ~_BV(pin - 24);
				PORTD |= _BV(pin - 24);
			} else if (pin < 40) {
				DDRE &= ~_BV(pin - 32);
				PORTE |= _BV(pin - 32);
			} else if (pin < 48) {
				DDRF &= ~_BV(pin - 40);
				PORTF |= _BV(pin - 40);
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
			} else if (pin < 40) {
				DDRE |= _BV(pin - 32);
			} else if (pin < 48) {
				DDRF |= _BV(pin - 40);
			}
		}
		inline void output(unsigned char const pin, unsigned char const value) {
			if (pin < 8) {
			} else if (pin < 16) {
				PORTB = value ? (PORTB | _BV(pin - 8)) : (PORTB & ~_BV(pin - 8));
				DDRB |= _BV(pin - 8);
			} else if (pin < 24) {
				PORTC = value ? (PORTC | _BV(pin - 16)) : (PORTC & ~_BV(pin - 16));
				DDRC |= _BV(pin - 16);
			} else if (pin < 32) {
				PORTD = value ? (PORTD | _BV(pin - 24)) : (PORTD & ~_BV(pin - 24));
				DDRD |= _BV(pin - 24);
			} else if (pin < 40) {
				PORTE = value ? (PORTE | _BV(pin - 32)) : (PORTE & ~_BV(pin - 32));
				DDRE |= _BV(pin - 32);
			} else if (pin < 48) {
				PORTF = value ? (PORTF | _BV(pin - 40)) : (PORTF & ~_BV(pin - 40));
				DDRF |= _BV(pin - 40);
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
			if (pin < 40) {
				return (PINE & _BV(pin - 32));
			}
			if (pin < 48) {
				return (PINF & _BV(pin - 40));
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
			} else if (pin < 40) {
				if (value) {
					PORTE |= _BV(pin - 32);
				} else {
					PORTE &= ~_BV(pin - 32);
				}
			} else if (pin < 48) {
				if (value) {
					PORTF |= _BV(pin - 40);
				} else {
					PORTF &= ~_BV(pin - 40);
				}
			}
		}
		inline void enable_int(unsigned char const pin) {
		}
		inline void disable_int(unsigned char const pin) {
		}
};

extern GPIO gpio;

#endif
