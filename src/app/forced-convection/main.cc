/*
 * Copyright 2026 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/stdin.h"
#include "driver/i2c.h"
#include "driver/soft_i2c.h"
#include "driver/lm75.h"
#include "driver/ssd1306.h"
#include "lib/pixelfont/pixeloperator_mirrored.h"
#include "object/framebuffer.h"

#include <string.h>

#define shift_ser GPIO::d8
#define shift_clk GPIO::d4
#define shift_latch GPIO::d12
#define shift_en GPIO::d7

#define PWM1 OCR2A
#define PWM2 OCR2B
#define PWM3 OCR0A
#define PWM4 OCR0B

#define PWM1_EN _BV(5)
#define PWM2_EN _BV(3)
#define PWM3_EN _BV(0)
#define PWM4_EN _BV(7)

unsigned char graph[88];
unsigned char graph_offset = 0;
char buffer[64];
unsigned char buf_pos = 0;
unsigned char pwm_max;

void loop(void)
{
	unsigned char buf_offset = 0;
	static int seconds = 0;
	static int fan_speed = 0;
	static int publish = 0;
	gpio.led_on(0);
	float temp = lm75.getTemp();

	if (PWM1 && (fan_speed != pwm_max)) {
		PWM1 = pwm_max;
		PWM2 = pwm_max;
		PWM3 = pwm_max;
		PWM4 = pwm_max;
	}

	if (temp < 25) {
		PWM1 = 0;
		PWM2 = 0;
		PWM3 = 0;
		PWM4 = 0;
		TCCR0A = 0;
		TCCR2A = 0;
		TCCR0B = 0;
		TCCR2B = 0;
		fan_speed = 0;
	}
	else if (temp >= 30) {
		PWM1 = pwm_max;
		PWM2 = pwm_max;
		PWM3 = pwm_max;
		PWM4 = pwm_max;
		TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
		TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
		TCCR0B = _BV(CS00);
		TCCR2B = _BV(CS20);
		fan_speed = pwm_max;
	}

	while (kin.hasKey()) {
		char key = kin.getKey();
		if ((key >= ' ') && (key <= '~')) {
			buffer[buf_pos++] = key;
		}
#if DEBUG
		kout << key << flush;
#endif
		gpio.led_toggle(1);

		if (buf_pos >= sizeof(buffer)) {
			buf_pos = 0;
		}

		if (buf_pos && ((key == '\n') || (key == '\r'))) {
			buffer[buf_pos] = 0;
			if (!strncmp(buffer, "RX:", 3)) {
				buf_offset = 3;
			} else if (!strncmp(buffer, "> RX:", 5)) {
				buf_offset = 5;
			} else {
				buf_offset = 0;
			}
			if (buf_offset) {
				pwm_max = 0;
				for (unsigned char i = buf_offset; (i < buf_pos) && buffer[i] ; i++) {
					if ((buffer[i] >= '0') && (buffer[i] <= '9')) {
						pwm_max *= 10;
						pwm_max += buffer[i] - '0';
					}
				}
			}
			buf_pos = 0;
		}
	}

	if (seconds++ == 60) {
		if (temp < 20) {
			graph[graph_offset ] = 0;
		}
		else if (temp > 40) {
			graph[graph_offset ] = 31;
		}
		else {
			graph[graph_offset] = (temp - 20) * 1.5;
		}
		seconds = 0;
		graph_offset = (graph_offset + 1) % (sizeof(graph) / sizeof(graph[0]));
	}

	uint16_t vcc = adc.getVCC_mV();

	fb.clear();
	fb.setFont(pixeloperator_mirrored);
	fb.setPos(88, 0);
	fb << temp << "c";
	fb.setPos(88, 16);
	fb << vcc/1000 << "." << (vcc%1000)/100 << (vcc%100)/10 << "V";
	fb.setPos(88, 24);
	fb << fan_speed;

	publish += 1;
	if (publish == 5) {
		kout << "publish_mqtt('sensor/hm17/schlafzimmer/heizung',";
		kout << "'{\"temperature_degc\":";
		kout.printf_float(temp);
		kout << ",\"fan_pwm\":" << fan_speed;
		kout << ",\"mcu_mV\":" << vcc << "}', 0);" << endl;
	}
	else if (publish == 10) {
		kout << "publish_influx('radiator,area=hm17,location=schlafzimmer temperature_degc=";
		kout.printf_float(temp);
		kout << ",fan_pwm=" << fan_speed << ",mcu_mV=" << vcc << "');" << endl;
		publish = 0;
	}

	for (unsigned char i = 0; i < (sizeof(graph) / sizeof(graph[0])); i++) {
		unsigned char column_value = graph[(graph_offset + i) % (sizeof(graph) / sizeof(graph[0]))];
		fb.data[i*4 + 3 - (column_value/8)] = 1 << ( 7 - column_value%8);
	}
	ssd1306.showImage(fb.data, fb.width * fb.height / 8);

	gpio.led_off(0);
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();
	kin.setup();

	gpio.led_on(0);

	pwm_max = 255;

	/*
	 * Ensure that the SSD1306 OLED display gets a clean reset / power cycle
	 */

	gpio.output(GPIO::a2, 0);
	gpio.output(GPIO::a3, 0);

	arch.delay_ms(50);

	gpio.write(GPIO::a2, 1);
	gpio.write(GPIO::a3, 0);

	arch.delay_ms(50);

	if (i2c.setup() != 0) {
		kout << "-- I2C setup FAILED" << endl;
		return 1;
	}
	kout << "-- I2C setup OK" << endl;

	if (softi2c.setup() != 0) {
		kout << "-- SoftI2C setup FAILED" << endl;
		return 1;
	}
	kout << "-- SoftI2C setup OK" << endl;

	lm75.setOS(30);
	lm75.setHyst(25);
	ssd1306.init();

	fb.setFont(pixeloperator_mirrored);

	gpio.output(shift_ser, 0);
	gpio.output(shift_clk, 0);
	gpio.output(shift_latch, 0);
	gpio.output(shift_en, 0);

	gpio.output(GPIO::pd3, 0); // OC2B - D3
	gpio.output(GPIO::pd5, 0); // OC0B - D5
	gpio.output(GPIO::pd6, 0); // OC0A - D6
	gpio.output(GPIO::pb3, 0); // OC2A - D11

	// m4a m2a m1a m1b m2b m3a m4b m3b
	uint8_t shift_out = 0b10101001;

	for (unsigned int i = 0; i < 8; i++) {
		gpio.write(shift_ser, shift_out & (1<<i));
		gpio.write(shift_clk, 1);
		gpio.write(shift_clk, 0);
	}
	gpio.write(shift_latch, 1);
	gpio.write(shift_latch, 0);

	gpio.led_off(0);

	arch.idle_loop();

	return 0;
}
