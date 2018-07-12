#include "arch.h"
#include <unistd.h>

void Arch::setup(void) { }

void Arch::idle_loop(void)
{
	while (1) {
		sleep(1);
	}
}

void Arch::idle(void)
{
}

Arch arch;
