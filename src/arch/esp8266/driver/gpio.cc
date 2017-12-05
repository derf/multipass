#include "driver/gpio.h"
extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
}

void ICACHE_FLASH_ATTR GPIO::setup()
{
	gpio_init();

	// Enable GPIO2 (ESP8266 on-board LED) as output
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

	// Enable GPIO16 (RTC out / NodeMCU on-board LED) as output
	//WRITE_PERI_REG(PAD_XPD_DCDC_CONF, (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);
	//WRITE_PERI_REG(RTC_GPIO_CONF, (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);
	//WRITE_PERI_REG(RTC_GPIO_ENABLE, (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);
}

void ICACHE_FLASH_ATTR GPIO::led_on(unsigned char id)
{
	gpio_output_set(0, BIT2, BIT2, 0);
}

void ICACHE_FLASH_ATTR GPIO::led_off(unsigned char id)
{
	gpio_output_set(BIT2, 0, BIT2, 0);
}

void ICACHE_FLASH_ATTR GPIO::led_toggle(unsigned char id)
{
	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2) {
		led_on(0);
	} else {
		led_off(0);
	}
}

GPIO gpio;
