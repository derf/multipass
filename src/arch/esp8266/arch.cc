#include "arch.h"

extern "C" {
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "gpio.h"
#include "mem.h"
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

extern int main(void);

void Arch::setup(void)
{
}

void Arch::idle_loop(void)
{
}

extern "C" void user_init(void)
{
	main();
}

Arch arch;
