/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

void loop(void)
{
	uint16_t vcc = adc.getVCC_mV();
	uint16_t temp = adc.getTemp_mdegC();

	kout << "Voltage     " << vcc << " mv" << endl;
	kout << "Temperature " << temp << " m°C" <<  endl;

	for (uint8_t admux_sel = 0; admux_sel < 8; admux_sel++) {
		kout << "ADC" << admux_sel << "        " << adc.getPin_mV(admux_sel, vcc) << " mV vs AVCC" << endl;
		kout << "ADC" << admux_sel << "        " << adc.getPin_mV(admux_sel) << " mV vs 1.1V bandgap" << endl;
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	arch.idle_loop();

	return 0;
}
