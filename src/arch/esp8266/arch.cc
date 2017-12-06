#include "arch.h"

extern "C" {
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "gpio.h"
#include "mem.h"
void ets_timer_arm_new(os_timer_t *ptimer, uint32_t milliseconds, bool repeat_flag, bool us_flag);
void ets_timer_disarm(os_timer_t *ptimer);
void ets_timer_setfn(os_timer_t *ptimer, os_timer_func_t *pfunction, void *parg);
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

#ifdef WITH_LOOP
LOCAL os_timer_t loop_timer;

extern void loop(void);
static void ICACHE_FLASH_ATTR jump_to_loop(void *arg)
{
	loop();
}

#endif

extern int main(void);

void ICACHE_FLASH_ATTR jump_to_main(void)
{
#ifdef WITH_LOOP
	os_timer_disarm(&loop_timer);
	os_timer_setfn(&loop_timer, (os_timer_func_t *)jump_to_loop, (void *)0);
	os_timer_arm(&loop_timer, 1000, 1);
#endif
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
