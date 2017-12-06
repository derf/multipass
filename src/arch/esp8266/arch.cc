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

static void ICACHE_FLASH_ATTR jump_to_main(void)
{
	main();
}

void Arch::setup(void)
{
}

void Arch::idle_loop(void)
{
}

extern "C" void user_init(void)
{
	system_init_done_cb(jump_to_main);
}

Arch arch;
