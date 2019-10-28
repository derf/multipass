#include "arch.h"
#include <msp430.h>

#ifdef __acweaving
#define __delay_cycles(x)
#endif

void Arch::setup(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	PJSEL0 = BIT4 | BIT5;

	PM5CTL0 &= ~LOCKLPM5;

	/*
	 * Note: arch drivers assume SMCLK freq == F_CPU
	 */

#if F_CPU == 16000000UL
	FRCTL0 = FWPW; // unlock FRAM Control
	FRCTL0_L = 0x10; // one wait state before FRAM access (required for 8MHz < F_CPU <= 16 MHz)
	FRCTL0_H = 0xff; // lock FRAM control by writing an invalid password

	// 16MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCORSEL | DCOFSEL_4;
#elif F_CPU == 8000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_6;
#elif F_CPU == 4000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_3;
#elif F_CPU == 1000000UL
	// 8MHz DCO
	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCOFSEL_0;
#else
#error Unsupported F_CPU
#endif

#ifdef WITH_LOOP
	CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
#else
	CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
#endif
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
	CSCTL0_H = 0;


#ifdef WITH_LOOP
	// enable LXFT for RTC
	CSCTL0_H = CSKEY >> 8;
	CSCTL4 &= ~LFXTOFF;
	while (SFRIFG1 & OFIFG) {
		CSCTL5 &= ~LFXTOFFG;
		SFRIFG1 &= ~OFIFG;
	}
	CSCTL0_H = 0;

	__delay_cycles(1000000);
#endif

#ifdef TIMER_US
#if F_CPU == 16000000UL
	TA0CTL = TASSEL__SMCLK | ID__8 | MC__CONTINUOUS; // /8
	TA0EX0 = 1; // /2 -> /16
#elif F_CPU == 8000000UL
	TA0CTL = TASSEL__SMCLK | ID__8 | MC__CONTINUOUS; // /8
	TA0EX0 = 0; // /1 -> /8
#elif F_CPU == 4000000UL
	TA0CTL = TASSEL__SMCLK | ID__4 | MC__CONTINUOUS; // /4
	TA0EX0 = 0; // /1 -> /8
#elif F_CPU == 1000000UL
	TA0CTL = TASSEL__SMCLK | ID__1 | MC__CONTINUOUS; // /1
	TA0EX0 = 0; // /1 -> /8
#else
#error Unsupported F_CPU
#endif /* F_CPU */
	TA0CTL |= TACLR;
#endif /* TIMER_US */

#if defined(WITH_LOOP) || defined(TIMER_S)
	// 1s per wakeup for loop. Independent of SMCLK/F_CPU
	TA1CTL = TASSEL__ACLK | ID__8 | MC__UP;
	TA1EX0 = 0;
	TA1CCR0 = 4096;
	TA1CTL |= TACLR | TAIE;
#endif

#ifdef TIMER_CYCLES
	TA2CTL = TASSEL__SMCLK | ID__1 | MC__CONTINUOUS;
	TA2EX0 = 0;
	TA2CTL |= TACLR;
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
	if (us < 10) {
		for (unsigned int i = 0; i < us; i++) {
			__delay_cycles(F_CPU / 1000000UL);
		}
	} else {
		for (unsigned int i = 0; i < us/10; i++) {
			__delay_cycles(F_CPU / 100000UL);
		}
	}
}
void Arch::delay_ms(unsigned int const ms)
{
	for (unsigned int i = 0; i < ms; i++) {
		__delay_cycles(F_CPU / 1000UL);
	}
}

volatile bool sleep_done = false;

// max delay: 2621 ms
void Arch::sleep_ms(unsigned int const ms)
{
	sleep_done = false;
#if F_CPU == 16000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 7; // /8 -> /64 (250 kHz)
#elif F_CPU == 8000000UL
	TA3CTL = TASSEL__SMCLK | ID__8; // /8
	TA3EX0 = 3; // /4 -> /32 (250 kHz)
#elif F_CPU == 4000000UL
	TA3CTL = TASSEL__SMCLK | ID__4; // /4
	TA3EX0 = 3; // /4 -> /16 (250 kHz)
#elif F_CPU == 1000000UL
	TA3CTL = TASSEL__SMCLK | ID__1; // /1
	TA3EX0 = 3; // /4 -> /4 (250 kHz)
#else
#error Unsupported F_CPU
#endif /* F_CPU */
	TA3CCR0 = ms * 250;
	TA3CCTL0 = CCIE;
	TA3CTL |= MC__UP | TACLR;
	while (!sleep_done) {
		asm volatile("nop");
		__bis_SR_register(GIE | LPM2_bits);
		asm volatile("nop");
		__dint();
	}
	TA3CTL = TASSEL__SMCLK;
}

void Arch::idle_loop(void)
{
	while (1) {
		asm volatile("nop");
		__bis_SR_register(GIE | LPM2_bits);
		asm volatile("nop");
		__dint();
#if defined(WITH_LOOP)
		if (run_loop) {
			loop();
			run_loop = 0;
		}
#endif
#ifdef WITH_WAKEUP
		wakeup();
#endif
	}
}

void Arch::idle(void)
{
	asm volatile("nop");
	__bis_SR_register(GIE | LPM2_bits);
	asm volatile("nop");
	__dint();
#ifdef WITH_WAKEUP
	wakeup();
#endif
}

Arch arch;

#if defined(WITH_LOOP) || defined(TIMER_S)

#include "driver/uptime.h"

#ifndef __acweaving
// overflow interrupts end up in A1 (joint interrupt for CCR1 ... CCR6 and overflow)
__attribute__((interrupt(TIMER1_A1_VECTOR))) __attribute__((wakeup)) void handle_timer1_overflow()
{
	if (TA1IV == 0x0e) {
#ifdef WITH_LOOP
		run_loop = 1;
#endif
#ifdef TIMER_S
		uptime.tick_s();
#endif
	}
}
#endif

#endif /* defined(WITH_LOOP) || defined(TIMER_S) */

#ifndef __acweaving
// CCR0 interrupts are exclusive to A0
__attribute__((interrupt(TIMER3_A0_VECTOR))) __attribute__((wakeup)) void handle_timer3_ccr0()
{
	sleep_done = true;
}
#endif /* __acweaving */
