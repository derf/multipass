#include "arch.h"
#include <msp430.h>

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
	// 16MHz/16 -> ~1MHz timer
	TA0CTL = TASSEL__SMCLK | ID__8 | MC__CONTINUOUS;
	TA0EX0 = 1;
	TA0CTL |= TACLR;
#endif

#if defined(WITH_LOOP) || defined(TIMER_S)
	// 1s per wakeup for loop
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

	//P1OUT = 0;
	//P4OUT = 0;
}

#ifdef WITH_WAKEUP
extern void wakeup();
#endif

#if defined(WITH_LOOP)
extern void loop();
volatile char run_loop = 0;
#endif

void Arch::delay_us(unsigned char const us)
{
	for (int i = 0; i < us/10; i++) {
		__delay_cycles(160);
	}
}
void Arch::delay_ms(unsigned char const ms)
{
	for (int i = 0; i < ms; i++) {
		__delay_cycles(16000);
	}
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
/*
void uart_setup(void)
{
	UCA0CTLW0 = UCSWRST | UCSSEL__SMCLK;
	UCA0MCTLW = UCOS16 | (2<<5) | 0xD600;
	UCA0BR0 = 104;

	UCA0IRCTL = 0;
	UCA0ABCTL = 0;

	P2SEL0 &= ~(BIT0 | BIT1);
	P2SEL1 |= BIT0 | BIT1;
	P2DIR |= BIT0;

	UCA0CTLW0 &= ~UCSWRST;

	UCA0IE |= UCRXIE;
}

void uart_putchar(char c)
{
	while (!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = c;

	if (c == '\n')
		uart_putchar('\r');
}

__attribute__((interrupt(USCI_A0_VECTOR))) void USCI_A0_ISR(void)
{
	static char prompt[64];
	static unsigned int prompt_pos = 0;

	char buf;
	unsigned char raw_p_pos, parse_p_pos;

	char parsed_prompt[64];
	unsigned char argc = 0;
	char *argv[32];

	if (UCA0IFG & UCRXIFG) {
		buf = UCA0RXBUF;
		if (buf == '\r') {

			uart_putchar('\n');
			if (prompt_pos > 0) {

				parse_p_pos = 0;
				argv[0] = parsed_prompt;

				for (raw_p_pos = 0; raw_p_pos < prompt_pos; raw_p_pos++) {
					if (prompt[raw_p_pos] != ' ') {
						parsed_prompt[parse_p_pos++] = prompt[raw_p_pos];
					} else if ((raw_p_pos > 0) && (prompt[raw_p_pos-1] != ' ')) {
						argc++;
						parsed_prompt[parse_p_pos++] = 0;
						argv[argc] = parsed_prompt + parse_p_pos;
					}
				}

				if (parse_p_pos < 64)
					parsed_prompt[parse_p_pos] = 0;
				else
					parsed_prompt[63] = 0;

				check_command(argc, argv);
				prompt_pos = 0;
				*prompt = 0;
			}
			uart_puts(COL_YELLOW "msp430fr5969" COL_GREEN " > " COL_RESET);

		} else if (buf == '\f') {

			uart_puts("\n" COL_YELLOW "msp430fr5969" COL_GREEN " > " COL_RESET);
			uart_nputs(prompt, prompt_pos);

		} else if (buf == 0x7f) {

			if (prompt_pos) {
				prompt_pos--;
				uart_puts("\e[D \e[D");
			}

		} else if (buf == 0x15) { // ^U

			for ( ; prompt_pos > 0; prompt_pos-- )
				uart_puts("\e[D \e[D");
			*prompt = 0;

		} else if (buf == 0x17) { // ^W

			for ( ; (prompt_pos > 0) && (prompt[prompt_pos] != ' '); prompt_pos-- )
				uart_puts("\e[D \e[D");
			for ( ; (prompt_pos > 0) && (prompt[prompt_pos-1] == ' '); prompt_pos-- )
				uart_puts("\e[D \e[D");
			prompt[prompt_pos] = 0;

		} else if (buf >= ' ') {

			if (prompt_pos < sizeof(prompt)-1) {
				prompt[prompt_pos++] = buf;
				uart_putchar(buf);
			}
		}
	}
}
*/
