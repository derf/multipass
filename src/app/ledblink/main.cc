#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

#ifndef TIMER_CYCLES
#error makeflag timer_cycles=1 required
#endif
#ifndef TIMER_S
#error makeflag timer_s=1 required
#endif

/*
void check_command(unsigned char argc, char** argv)
{
	unsigned char i2c_rxbuf[16];
	unsigned char i2c_txbuf[16];
	float buf = 0;
	int i;
	if (!strcmp(argv[0], "i2c")) {
		if (argc == 0) {
			uart_puterr("Usage: i2c <on|off|detect|gettemp> [-u]\n");
			return;
		}
		if (!strcmp(argv[1], "on")) {
			if ((argc >= 2) && !strcmp(argv[2], "-u")) {
				if (i2c_setup(1) < 0)
					uart_puterr("Error initializing I²C: Line is busy\n");
			} else {
				if (i2c_setup(0) < 0)
					uart_puterr("Error initializing I²C: Line is busy\n"
							"Do you have hardware pullups on SDA/SCL?\n");
			}
		} else if (!strcmp(argv[1], "off")) {
			uart_puterr("Error: not implemented yet\n");
		} else if (!strcmp(argv[1], "detect")) {
			i2c_scan();
		} else if (!strcmp(argv[1], "tc74")) {
			i2c_txbuf[0] = 0x00;
			i2c_xmit(0x4d, 1, 1, i2c_txbuf, i2c_rxbuf);
			uart_putint(i2c_rxbuf[0]);
			uart_puts("°C\n");
		} else if (!strcmp(argv[1], "lm75")) {
			i2c_txbuf[0] = 0x00;
			i2c_xmit(0x48, 1, 2, i2c_txbuf, i2c_rxbuf);
			uart_putfloat(i2c_rxbuf[0] + (i2c_rxbuf[1] / 256.0));
			uart_puts("°C\n");
		}
		else if (!strcmp(argv[1], "eepr")) {
			i2c_rxbuf[0] = 0;
			i2c_txbuf[0] = 0;
			i2c_txbuf[1] = argv[2][0];
			i2c_xmit(0x50, 2, 1, i2c_txbuf, i2c_rxbuf);
			uart_putint(i2c_rxbuf[0]);
			uart_puts("\n");
		}
		else if (!strcmp(argv[1], "eepw")) {
			i2c_txbuf[0] = 0;
			i2c_txbuf[1] = argv[2][0];
			i2c_txbuf[2] = argv[3][0];
			i2c_txbuf[3] = argv[3][1];
			i2c_txbuf[4] = argv[3][2];
			i2c_txbuf[5] = argv[3][3];
			i2c_txbuf[6] = argv[3][4];
			i2c_txbuf[7] = argv[3][5];
			i2c_xmit(0x50, 8, 0, i2c_txbuf, i2c_rxbuf);
		}
	} else if (!strcmp(argv[0], "sensors")) {
		for (i = 0; i < 32; i++) {
			buf += adc_gettemp();
			__delay_cycles(64000);
		}
		uart_puts("Temperature : ");
		uart_putfloat(buf / 32);
		uart_puts("°C avg / ");
		uart_putfloat(adc_gettemp());
		uart_puts("°C single\n    Voltage : ");
		uart_putfloat(adc_getvcc());
		uart_puts("V\n");
	} else if (!strcmp(argv[0], "spi")) {
		if (argc == 0) {
			uart_puterr("Usage: spi <on|off|sharp>\n");
			return;
		}
		if (!strcmp(argv[1], "on")) {
			spi_setup();
		}
	} else if (!strcmp(argv[0], "help")) {
		uart_puts("Supported commands: i2c sensors\n");
	} else {
		uart_puterr("Unknown command\n");
	}
}
*/

void loop(void)
{
	gpio.led_toggle(1);
	kout << dec << uptime.get_s() << endl;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.led_on(0);
	kout << "Hello, World!" << endl;
	kout << "Test, World!" << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;
	kout << dec << uptime.get_cycles() << endl;

	arch.idle_loop();

	//uart_setup();
	//uart_puts("\n" COL_YELLOW "dOS" COL_GREEN " > " COL_RESET);

	//arch_idle_loop();

	return 0;
}
