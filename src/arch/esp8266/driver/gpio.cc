/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "driver/gpio.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
}

void ICACHE_FLASH_ATTR GPIO::setup()
{
	gpio_init();

	// Enable GPIO2 (ESP8266 on-board LED) as output
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

#ifdef LED_ON_GPIO16
	// Enable GPIO16 (RTC out / NodeMCU on-board LED) as output
	WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);
	WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);
	WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);
	// Turn the GPIO on to make sure the LED is off by default
	WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(1));
#endif
}

void ICACHE_FLASH_ATTR GPIO::led_on(unsigned char const id)
{
#ifdef LED_ON_GPIO16
	if (id == 0) {
		gpio_output_set(0, BIT2, BIT2, 0);
	} else {
		WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(0));
	}
#else
	gpio_output_set(0, BIT2, BIT2, 0);
#endif
}

void ICACHE_FLASH_ATTR GPIO::led_off(unsigned char const id)
{
#ifdef LED_ON_GPIO16
	if (id == 0) {
		gpio_output_set(BIT2, 0, BIT2, 0);
	} else {
		WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(0));
	}
#else
	gpio_output_set(BIT2, 0, BIT2, 0);
#endif
}

void ICACHE_FLASH_ATTR GPIO::led_toggle(unsigned char const id)
{
#ifdef LED_ON_GPIO16
	if (id == 0) {
		if (gpio_input_get() & BIT2) {
			led_on(0);
		} else {
			led_off(0);
		}
	} else {
		WRITE_PERI_REG(RTC_GPIO_OUT, (READ_PERI_REG(RTC_GPIO_OUT) ^ BIT0));
	}
#else
	if (gpio_input_get() & BIT2) {
		led_on(0);
	} else {
		led_off(0);
	}
#endif
}

void ICACHE_FLASH_ATTR GPIO::input(unsigned char const pin)
{
	if (pin == d0) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	} else if (pin == tx) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
	} else if (pin == d4) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	} else if (pin == rx) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
	} else if (pin == d2) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	} else if (pin == d1) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	} else if (pin == d6) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	} else if (pin == d7) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	} else if (pin == d5) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	} else if (pin == d8) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
	} else if (pin == d0) {
		// TODO this sets it as output, not input
		WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);
		WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);
		WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);
	}
	if (pin < d0) {
		gpio_output_set(0, 0, 0, (1 << pin));
	}
}

void ICACHE_FLASH_ATTR GPIO::input(unsigned char const pin, bool pullup)
{
	if (pin == d0) {
		if (pullup) {
			PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);
		} else {
			PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);
		}
	} else if (pin == tx) {
		if (pullup) {
			PIN_PULLUP_EN(PERIPHS_IO_MUX_U0TXD_U);
		} else {
			PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
		}
	} else if (pin == d4) {
		if (pullup) {
			PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);
		} else {
			PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO2_U);
		}
	} else if (pin == rx) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
	} else if (pin == d2) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	} else if (pin == d1) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	} else if (pin == d6) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	} else if (pin == d7) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	} else if (pin == d5) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	} else if (pin == d8) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
	} else if (pin == d0) {
		// TODO this sets it as output, not input
		WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);
		WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);
		WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);
	}
	input(pin);
}

void ICACHE_FLASH_ATTR GPIO::output(unsigned char const pin)
{
	if (pin == d0) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	} else if (pin == tx) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
	} else if (pin == d4) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	} else if (pin == rx) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
	} else if (pin == d2) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	} else if (pin == d1) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	} else if (pin == d6) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	} else if (pin == d7) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	} else if (pin == d5) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	} else if (pin == d8) {
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);
	} else if (pin == d0) {
		WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);
		WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);
		WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);
	}
	if (pin < d0) {
		gpio_output_set(0, 0, (1 << pin), 0);
	}
}

unsigned char ICACHE_FLASH_ATTR GPIO::read(unsigned char const pin)
{
	return (gpio_input_get() & (1 << pin));
}

void ICACHE_FLASH_ATTR GPIO::write(unsigned char const pin, unsigned char value)
{
	if (value) {
		gpio_output_set(1 << pin, 0, 0, 0);
	} else {
		gpio_output_set(0, 1 << pin, 0, 0);
	}
}

GPIO gpio;
