/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

void loop(void)
{
#ifdef CONFIG_arch_arduino_nano
	uint16_t vcc = adc.getVCC_mV();
	uint16_t temp = adc.getTemp_mdegC();

	kout << "Voltage     " << vcc << " mV" << endl;
	kout << "Temperature " << temp << " m°C" <<  endl;

	for (uint8_t admux_sel = 0; admux_sel < 8; admux_sel++) {
		kout << "ADC" << admux_sel << "        " << adc.getPin_mV(admux_sel, vcc) << " mV vs AVCC" << endl;
		kout << "ADC" << admux_sel << "        " << adc.getPin_mV(admux_sel) << " mV vs 1.1V bandgap" << endl;
	}
#endif
#ifdef CONFIG_arch_msp430fr5994lp
	float vcc = adc.getVCC();
	float temp = adc.getTemp();

	kout << "Voltage     " <<  vcc << " V" << endl;
	kout << "Temperature " << temp << " °C" <<  endl;
#endif
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	arch.idle_loop();

	return 0;
}
