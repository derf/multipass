/*
 * Copyright 2021 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/lm4f/systemcontrol.h>
#include <libopencm3/lm4f/rcc.h>
#include <libopencm3/lm4f/gpio.h>
#include <libopencm3/lm4f/nvic.h>
#include <libopencm3/lm4f/uart.h>

void arch_clock_init()
{
	// 16MHz Crystal -> 400 MHz PLL -> 80 MHz (/5)
	rcc_sysclk_config(OSCSRC_MOSC, XTAL_16M, 5);
}

#ifdef CONFIG_loop
volatile char run_loop = 1;

void arch_init_loop()
{
	systick_clear();
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_frequency(2, F_CPU);
	systick_counter_enable();
	systick_interrupt_enable();

	//periph_clock_enable(RCC_TIMER3);
}

void sys_tick_handler()
{
	run_loop = 1;
}
#endif

void mp_uart_setup()
{
	gpio_set_af(GPIOA, 1, GPIO0 | GPIO1);

	periph_clock_enable(RCC_UART0);

	__asm__("nop");

	uart_disable(UART0);

	uart_clock_from_piosc(UART0);

	uart_set_baudrate(UART0, 9600);
	uart_set_databits(UART0, 8);
	uart_set_parity(UART0, UART_PARITY_NONE);
	uart_set_stopbits(UART0, 1);

	uart_enable(UART0);
}

void mp_uart_send_blocking(char c)
{
	uart_send_blocking(UART0, c);
}

#define pb_0 8
#define pc_0 16
#define pd_0 24
#define pe_0 32
#define pf_0 40
#define PIN_INVALID 45

void mp_gpio_setup()
{
	const unsigned int outpins = GPIO1 | GPIO3 | GPIO2;

	gpio_enable_ahb_aperture();

	periph_clock_enable(RCC_GPIOA);
	periph_clock_enable(RCC_GPIOB);
	periph_clock_enable(RCC_GPIOC);
	periph_clock_enable(RCC_GPIOD);
	periph_clock_enable(RCC_GPIOE);
	periph_clock_enable(RCC_GPIOF);

	gpio_mode_setup(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, outpins);
	gpio_set_output_config(GPIOF, GPIO_OTYPE_PP, GPIO_DRIVE_2MA, outpins);
}

void mp_gpio_input(unsigned char const pin)
{
	if (pin < pb_0) {
		gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << pin);
	} else if (pin < pc_0) {
		gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pb_0));
	} else if (pin < pd_0) {
		gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pc_0));
	} else if (pin < pe_0) {
		gpio_mode_setup(GPIOD, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pd_0));
	} else if (pin < pf_0) {
		gpio_mode_setup(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pe_0));
	} else if (pin < PIN_INVALID) {
		gpio_mode_setup(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_NONE, 1 << (pin - pf_0));
	}
}

void mp_gpio_output(unsigned char const pin)
{
	if (pin < pb_0) {
		gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << pin);
	} else if (pin < pc_0) {
		gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pb_0));
	} else if (pin < pd_0) {
		gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pc_0));
	} else if (pin < pe_0) {
		gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pd_0));
	} else if (pin < pf_0) {
		gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pe_0));
	} else if (pin < PIN_INVALID) {
		gpio_mode_setup(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, 1 << (pin - pf_0));
	}
}

unsigned int mp_gpio_read(unsigned char const pin) {
	if (pin < pb_0) {
		return gpio_read(GPIOA, 1 << pin);
	} else if (pin < pc_0) {
		return gpio_read(GPIOB, 1 << (pin-pb_0));
	} else if (pin < pd_0) {
		return gpio_read(GPIOC, 1 << (pin-pc_0));
	} else if (pin < pe_0) {
		return gpio_read(GPIOD, 1 << (pin-pd_0));
	} else if (pin < pf_0) {
		return gpio_read(GPIOE, 1 << (pin-pe_0));
	} else if (pin < PIN_INVALID) {
		return gpio_read(GPIOF, 1 << (pin-pf_0));
	}
}

void mp_gpio_write(unsigned char const pin, unsigned char value) {
	if (pin < pb_0) {
		if (value) {
			gpio_port_write(GPIOA, gpio_port_read(GPIOA) | (1 << pin));
		} else {
			gpio_port_write(GPIOA, gpio_port_read(GPIOA) & ~(1 << pin));
		}
	} else if (pin < pc_0) {
		if (value) {
			gpio_port_write(GPIOB, gpio_port_read(GPIOB) | (1 << (pin-pb_0)));
		} else {
			gpio_port_write(GPIOB, gpio_port_read(GPIOB) & ~(1 << (pin-pb_0)));
		}
	} else if (pin < pd_0) {
		if (value) {
			gpio_port_write(GPIOC, gpio_port_read(GPIOC) | (1 << (pin-pc_0)));
		} else {
			gpio_port_write(GPIOC, gpio_port_read(GPIOC) & ~(1 << (pin-pc_0)));
		}
	} else if (pin < pe_0) {
		if (value) {
			gpio_port_write(GPIOD, gpio_port_read(GPIOD) | (1 << (pin-pd_0)));
		} else {
			gpio_port_write(GPIOD, gpio_port_read(GPIOD) & ~(1 << (pin-pd_0)));
		}
	} else if (pin < pf_0) {
		if (value) {
			gpio_port_write(GPIOE, gpio_port_read(GPIOE) | (1 << (pin-pe_0)));
		} else {
			gpio_port_write(GPIOE, gpio_port_read(GPIOE) & ~(1 << (pin-pe_0)));
		}
	} else if (pin < PIN_INVALID) {
		if (value) {
			gpio_port_write(GPIOF, gpio_port_read(GPIOF) | (1 << (pin-pf_0)));
		} else {
			gpio_port_write(GPIOF, gpio_port_read(GPIOF) & ~(1 << (pin-pf_0)));
		}
	}
}
