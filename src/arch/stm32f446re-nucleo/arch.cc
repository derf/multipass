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
	// NUCLEO-F443RE uses 8MHz STLINK clock as input
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
#ifdef WITH_LOOP
	rcc_periph_clock_enable(RCC_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);
	rcc_periph_reset_pulse(RST_TIM2);


	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	// 10 kHz timer frequency
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 10000));

	timer_disable_preload(TIM2); // ?
	timer_continuous_mode(TIM2); // ?

	timer_set_period(TIM2, 10000);

	timer_enable_counter(TIM2);
	timer_enable_irq(TIM2, TIM_DIER_UIE);
#endif
}

#ifdef WITH_WAKEUP
extern void wakeup();
#endif

#if defined(WITH_LOOP)
extern void loop();
volatile char run_loop = 0;
#endif

void Arch::delay_us(unsigned int const us)
{
	volatile int x = us * 2;
	while (x--) {
		__asm("nop");
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		volatile int x = 2000;
		while (x--) {
			__asm("nop");
		}
	}
}

void Arch::idle_loop(void)
{
	while (1) {
		pwr_set_standby_mode();
		__asm__("wfi");
#ifdef WITH_LOOP
		if (run_loop) {
			loop();
			run_loop = 0;
		}
#endif
	}
}

void Arch::idle(void)
{
#ifdef WITH_WAKEUP
	wakeup();
#endif
}

Arch arch;

#if defined(WITH_LOOP) || defined(TIMER_S)

#include "driver/uptime.h"

void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_UIF)) {
		timer_clear_flag(TIM2, TIM_SR_UIF);
#ifdef WITH_LOOP
		run_loop = 1;
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}
#endif
