/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/neopixel.h"
#include "driver/stdout.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define NUM_PIXELS 27

Adafruit_NeoPixel np(NUM_PIXELS, GPIO::pb0, NEO_GRB+NEO_KHZ800);

class Blinkencat {
	private:
		uint8_t btn_debounce;

	public:
		enum Mode : uint8_t {
			OFF = 0,
			CHARGE_LEVEL,
			RGBWHEEL_FAST,
			RGBWHEEL_SLOW,
			RGBFADE_FAST,
			RGBFADE_SLOW,
			BRIGHTRGBWHEEL_FAST,
			BRIGHTRGBWHEEL_SLOW,
			BRIGHTRGBFADE_FAST,
			BRIGHTRGBFADE_SLOW,
			COLD_WHITE,
			STROBE,
			COLOR_STROBE,
			MODE_ENUM_MAX
		};

		Mode mode;
		uint16_t vcc;

		void setup(void);
		void next_mode(void);
		void debounce_done(void);
		void debounce_start(void);
		void check_battery(void);
		void sleep(void);
		void idle(void);
		void loop(void);

		Blinkencat() : btn_debounce(0), mode(OFF), vcc(0) {}
};

void Blinkencat::setup(void)
{
	np.setup();
	gpio.input(GPIO::pb1, 0); // LED GND (LED has no resistor, do not set to output!)
	gpio.input(GPIO::pd3, 1); // Button A
	gpio.enable_int(GPIO::pd3);

	// One ADC conversion per four seconds
	TCCR1A = 0;
	TCCR1B = _BV(CS12) | _BV(CS10);

	// Measure internal 1.1V bandgap using VCC as reference on each Timer 1 overflow
	ADMUX = _BV(REFS0) | 0x0e;
	ADCSRB = _BV(ADTS2) | _BV(ADTS1);
	ADCSRA = _BV(ADEN) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1);
}

void Blinkencat::idle(void)
{
	SMCR = _BV(SE);
	asm("sleep");
	SMCR = 0;
}

void Blinkencat::sleep(void)
{
	SMCR = _BV(SM1) | _BV(SE);
	asm("sleep");
	SMCR = 0;
}

void Blinkencat::debounce_start(void)
{
	if (!btn_debounce) {
		btn_debounce = 1;
		wdt_reset();
		WDTCSR = _BV(WDE) | _BV(WDCE);
		WDTCSR = _BV(WDIE) | _BV(WDP2);
	}
}

void Blinkencat::debounce_done(void)
{
	btn_debounce = 0;
	wdt_disable();
	// long press? -> turn off
	if (!gpio.read(GPIO::pd3)) {
		mode = OFF;
	}
}

void Blinkencat::next_mode(void)
{
	if (!btn_debounce) {
		mode = (Mode)((mode + 1) % MODE_ENUM_MAX);
	}
}

void Blinkencat::check_battery(void)
{
	if (ADCSRA & _BV(ADIF)) {
		uint8_t adcr_l = ADCL;
		uint8_t adcr_h = ADCH;
		uint16_t adcr = adcr_l + (adcr_h << 8);
		vcc = 1100L * 1023 / adcr;

		TIFR1 |= _BV(TOV1);
		ADCSRA |= _BV(ADIF);

		//kout << "VCC is " << vcc << endl;
		// 3.1 V under load ~~ 3.5 V idle
		if (vcc < 3000) {
			for (uint8_t i = 0; i < 5; i++) {
				for (uint8_t i = 0; i < NUM_PIXELS; i++) {
					np.setPixelColor(i, np.Color(0, 0, 0));
				}
				np.show();
				_delay_ms(400);
				for (uint8_t i = 0; i < NUM_PIXELS; i++) {
					np.setPixelColor(i, np.Color(127 * ((i % 7) == 0), 0, 0));
				}
				np.show();
				_delay_ms(400);
			}
			for (uint8_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(0, 0, 0));
			}
			np.show();
			sleep();
		}

		/*
		 * Both battery and Arduino Nano are connected to the output of the
		 * TP4056 LiIon charge controller, so it decides on the battery charge
		 * status based on their cumulative current draw. A sufficiently high
		 * Arduino Nano (WS2812B) current will cause it to charge indefinitely,
		 * shortening the LiIon battery's life span and increasing fire risk.
		 *
		 * To avoid this, we disable all LEDs when a charger is connected.
		 * This is preceeded by a 5x green flash to indicate that it is
		 * intended behaviour.
		 *
		 * VCC > 4.22 V indicates that the battery is charging.
		 * (float voltage without charger rarely exceeds 4.2 V)
		 */
		if (vcc > 4220) {
			for (uint8_t i = 0; i < 5; i++) {
				for (uint8_t i = 0; i < NUM_PIXELS; i++) {
					np.setPixelColor(i, np.Color(0, 0, 0));
				}
				np.show();
				_delay_ms(400);
				for (uint8_t i = 0; i < NUM_PIXELS; i++) {
					np.setPixelColor(i, np.Color(0, 200 * ((i % 7) == 0), 0));
				}
				np.show();
				_delay_ms(400);
			}
			for (uint8_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(0, 0, 0));
			}
			np.show();
			sleep();
		}
	}
}

void Blinkencat::loop(void)
{
	static uint16_t rgbwheel_offset = 0;
	static uint16_t rgbfade_hsv = 0;
	static uint8_t strobe_on = 0;

	switch (mode) {
		case OFF:
			// the mode may have been set by an ISR, which may in turn have
			// been handled immediately after an np.show() call. So we must
			// observe the 300us idle time mandated by WS2812.
			_delay_ms(1);
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(0, 0, 0));
			}
			np.show();
			sleep();
			break;
		case CHARGE_LEVEL:
			for (uint8_t i = 0; i < NUM_PIXELS; i++) {
				if (i < ((vcc - 3400) * NUM_PIXELS / 700)) {
					np.setPixelColor(i, np.Color((vcc < 3850) * 200, (vcc > 3700) * 200, 0));
				}
			}
			np.show();
			_delay_ms(2000);
			mode = RGBWHEEL_FAST;
			// fall-through
		case RGBWHEEL_FAST:
		case RGBWHEEL_SLOW:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				uint16_t hsv = (i * 252 + rgbwheel_offset) % 6553;
				np.setPixelColor((NUM_PIXELS-1) - i, np.gamma32(np.ColorHSV(hsv * 10, 255, 127)));
			}
			rgbwheel_offset = (rgbwheel_offset + 10) % 6553;
			np.show();
			_delay_ms(1);
			if (mode == RGBWHEEL_SLOW) {
				_delay_ms(9);
			}
			break;
		case RGBFADE_FAST:
		case RGBFADE_SLOW:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.ColorHSV(rgbfade_hsv * 10, 255, 63));
			}
			rgbfade_hsv = (rgbfade_hsv + 10) % 6553;
			np.show();
			_delay_ms(1);
			if (mode == RGBFADE_SLOW) {
				_delay_ms(99);
			}
			break;
		case BRIGHTRGBWHEEL_FAST:
		case BRIGHTRGBWHEEL_SLOW:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				uint16_t hsv = (i * 252 + rgbwheel_offset) % 6553;
				np.setPixelColor((NUM_PIXELS-1) - i, np.gamma32(np.ColorHSV(hsv * 10)));
			}
			rgbwheel_offset = (rgbwheel_offset + 10) % 6553;
			np.show();
			_delay_ms(1);
			if (mode == BRIGHTRGBWHEEL_SLOW) {
				_delay_ms(9);
			}
			break;
		case BRIGHTRGBFADE_FAST:
		case BRIGHTRGBFADE_SLOW:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.ColorHSV(rgbfade_hsv * 10));
			}
			rgbfade_hsv = (rgbfade_hsv + 10) % 6553;
			np.show();
			_delay_ms(1);
			if (mode == BRIGHTRGBFADE_SLOW) {
				_delay_ms(99);
			}
			break;
		case COLD_WHITE:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(127, 127, 127));
			}
			np.show();
			sleep();
			break;
		case STROBE:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(strobe_on, strobe_on, strobe_on));
			}
			np.show();
			strobe_on = 127 - strobe_on;
			_delay_ms(40);
			break;
		case COLOR_STROBE:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				if (strobe_on) {
					np.setPixelColor(i, np.ColorHSV(rgbfade_hsv * 10));
				} else {
					np.setPixelColor(i, np.Color(0, 0, 0));
				}
			}
			rgbfade_hsv = (rgbfade_hsv + 50) % 6553;
			np.show();
			strobe_on = 127 - strobe_on;
			_delay_ms(40);
			break;
	}
}

Blinkencat blinkencat;

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	blinkencat.setup();

	while (1) {
		blinkencat.check_battery();
		blinkencat.loop();
	}

	return 0;
}


ISR(WDT_vect)
{
	blinkencat.debounce_done();
}


ISR(PCINT2_vect)
{
	if (!gpio.read(GPIO::pd3)) {
		blinkencat.next_mode();
	}
	blinkencat.debounce_start();
}
