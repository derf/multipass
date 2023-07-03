/*
 * Copyright 2021 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
void arch_clock_init();
void arch_init_loop();

void mp_uart_setup();
void mp_uart_send_blocking(char c);

void mp_gpio_setup();
void mp_gpio_input(unsigned char const pin);
void mp_gpio_output(unsigned char const pin);
unsigned int mp_gpio_read(unsigned char const pin);
void mp_gpio_write(unsigned char const pin, unsigned char value);
