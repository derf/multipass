#include "driver/stdin.h"
#include <msp430.h>

void StandardInput::setup()
{
	UCA0IE |= UCRXIE;
}

bool StandardInput::hasKey()
{
	if (write_pos != read_pos) {
		return true;
	}
	return false;
}

char StandardInput::getKey()
{
	char ret = buffer[read_pos++];
	read_pos %= 8;
	return ret;
}

StandardInput kin;

__attribute__((interrupt(USCI_A0_VECTOR))) __attribute__((wakeup)) void handle_stdin()
{
	if (UCA0IFG & UCRXIFG) {
		kin.addKey(UCA0RXBUF);
	}
}
