#include "arch.h"
#include "driver/neopixel.h"
#include "driver/stdin.h"
#include "driver/stdout.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define NUM_PIXELS 66

Adafruit_NeoPixel np(NUM_PIXELS, GPIO::pb0, NEO_GRB+NEO_KHZ800);

class Blinkencat {
	public:
		enum Mode : uint8_t {
			OFF = 0,
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
			COLOR_RGB,
			MODE_ENUM_MAX
		};

		Mode mode;

		uint8_t red, green, blue;

		void setup(void);
		void next_mode(void);
		void idle(void);
		void loop(void);

		Blinkencat() : mode(OFF) {}
};

void Blinkencat::setup(void)
{
	np.setup();
	gpio.input(GPIO::pb1, 0);
	gpio.input(GPIO::pd3, 1);
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

void Blinkencat::next_mode(void)
{
	mode = (Mode)((mode + 1) % MODE_ENUM_MAX);
}

void Blinkencat::loop(void)
{
	static uint16_t rgbwheel_offset = 0;
	static uint16_t rgbfade_hsv = 0;
	static uint8_t strobe_on = 0;

	/*
	// not working due to bad logic levels
	if (gpio.read(GPIO::pb1)) {
		// Arduino and WS2812 strip are connected in parallel with the battery,
		// which will significantly confuse the charging circuit when the
		// strip is active while charging. So we make sure that it isn't.
		mode = OFF;
	}
	*/

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
			idle();
			break;
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
			idle();
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
		case COLOR_RGB:
			for (uint16_t i = 0; i < NUM_PIXELS; i++) {
				np.setPixelColor(i, np.Color(red, green, blue));
			}
			np.show();
			idle();
			break;
	}
}

Blinkencat blinkencat;

int main(void)
{
	char buf[3];
	unsigned char buf_pos = sizeof(buf);
	Blinkencat::Mode target_mode = blinkencat.OFF;

	arch.setup();
	gpio.setup();
	kout.setup();
	kin.setup();

	blinkencat.setup();

	while (1) {
		kout << "loop - mode is " << blinkencat.mode << endl;
		blinkencat.loop();
		while (kin.hasKey()) {
			char key = kin.getKey();

			if (buf_pos < sizeof(buf)) {
				buf[buf_pos] = key;
				buf_pos++;
				if (buf_pos == sizeof(buf)) {
					blinkencat.red = buf[0];
					blinkencat.green = buf[1];
					blinkencat.blue = buf[2];
					blinkencat.mode = target_mode;
				}
			}
			else if ((key >= '0') && (key < '0' + blinkencat.MODE_ENUM_MAX)) {
				blinkencat.mode = (Blinkencat::Mode)(key - '0');
			}
			else if (key == 'c') {
				buf_pos = 0;
				target_mode = blinkencat.COLOR_RGB;
			}
		}
		if (ADCSRA & _BV(ADIF)) {
			uint8_t adcr_l = ADCL;
			uint8_t adcr_h = ADCH;
			uint16_t adcr = adcr_l + (adcr_h << 8);
			uint16_t vcc = 1100L * 1023 / adcr;

			TIFR1 |= _BV(TOV1);
			ADCSRA |= _BV(ADIF);

			kout << "VCC = " << vcc << endl;
		}
	}

	return 0;
}
