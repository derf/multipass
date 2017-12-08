#include "driver/stdout.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
void uart_div_modify(uint8_t uart_no, uint32 DivLatchValue);
void os_printf_plus(const char *s, ...);
}

void StandardOutput::setup()
{
	uart_div_modify(0, UART_CLK_FREQ / 115200);
}

void StandardOutput::put(char c)
{
	os_printf("%c", c);
}

void StandardOutput::write(const char *s)
{
	os_printf("%s", s);
}

StandardOutput kout;
