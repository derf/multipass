/*
 * Copyright 2024 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "arch.h"

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
#if F_CPU == 216000000UL
	rcc_clock_setup_hsi(&rcc_3v3[RCC_CLOCK_3V3_216MHZ]);
#else
#error Unsupported F_CPU
#endif

	// counter
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	rcc_periph_reset_pulse(RST_TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM2, 0);
	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, 4294967295);
	timer_enable_irq(TIM2, TIM_DIER_UIE);

#ifdef CONFIG_loop
	rcc_periph_clock_enable(RCC_TIM3);
	nvic_enable_irq(NVIC_TIM3_IRQ);
	rcc_periph_reset_pulse(RST_TIM3);


	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	// 10 kHz timer frequency
	timer_set_prescaler(TIM3, ((rcc_apb1_frequency * 2) / 10000));

	timer_continuous_mode(TIM3); // ?

	timer_set_period(TIM3, 10000);

	timer_enable_counter(TIM3);
	timer_enable_irq(TIM3, TIM_DIER_UIE);
#endif
}

#ifdef CONFIG_wakeup
extern void wakeup();
#endif

#if defined(CONFIG_loop)
extern void loop();
volatile char run_loop = 0;
#endif

// for 216 MHz(?)
void Arch::delay_us(unsigned int const us)
{
	volatile int x = us * 145;
	while (x--) {
		__asm("nop");
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		volatile int x = 143990;
		while (x--) {
			__asm("nop");
		}
	}
}

void Arch::idle_loop(void)
{
	while (1) {
		//pwr_set_standby_mode();
		__asm__("wfi");
#ifdef CONFIG_loop
		if (run_loop) {
			loop();
			run_loop = 0;
		}
#endif
	}
}

void Arch::idle(void)
{
#ifdef CONFIG_wakeup
	wakeup();
#endif
}

Arch arch;

#if defined(CONFIG_loop) || defined(TIMER_S)

#include "driver/uptime.h"

void tim3_isr(void)
{
	if (timer_get_flag(TIM3, TIM_SR_UIF)) {
		timer_clear_flag(TIM3, TIM_SR_UIF);
#ifdef CONFIG_loop
		run_loop = 1;
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}
#endif