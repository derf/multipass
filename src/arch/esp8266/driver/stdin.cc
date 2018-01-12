#include "driver/stdin.h"
#include "driver/gpio.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "ets_sys.h"
void ets_isr_attach(uint16_t idx, void func(), void *arg);
void ets_isr_unmask(uint16_t idx);
}

#define USF  *((volatile uint32_t *)(0x60000000))
#define USIR *((volatile uint32_t *)(0x60000004))
#define USIS *((volatile uint32_t *)(0x60000008))
#define USIE *((volatile uint32_t *)(0x6000000c))
#define USIC *((volatile uint32_t *)(0x60000010))
#define USS  *((volatile uint32_t *)(0x6000001c))
#define USC1 *((volatile uint32_t *)(0x60000024))

#define UIFF 0
#define UIFE 1
#define UIPE 2
#define UIFR 3
#define UIOF 4
#define UIDSR 5
#define UICTS 6
#define UIBD 7
#define UITO 8

#define USRXC 0
#define UCFFT 0
#define UCTOT 24
#define UCTOE 31


#ifdef WITH_WAKEUP
void wakeup();
#endif

void uart_isr()
{
	if (USIS & ((1 << UIFF) | (1 << UITO))) {
		while ((USS >> USRXC) & 0x7f) {
			kin.addKey(USF);
		}
	}
	USIC = USIS;
#ifdef WITH_WAKEUP
	wakeup();
#endif
}

void StandardInput::setup()
{
	//USC1 = (0x7f << UCFFT) | (0x02 << UCTOT) | (1 << UCTOE );
	USIC = 0xffff;
	USIE = (1 << UIFF) | (1 << UIFR) | (1 << UITO);
	ETS_UART_INTR_ATTACH(&uart_isr, NULL);
	ETS_UART_INTR_ENABLE();
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
