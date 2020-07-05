#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"

#include <avr/interrupt.h>

volatile uint16_t anemometer_count = 0;

void loop(void)
{
	static uint8_t loop_count = 0;
	static uint16_t anemometer_copy;
	if (++loop_count == 10) {

		cli();
		anemometer_copy = anemometer_count;
		anemometer_count = 0;
		sei();

		kout << "Anemometer Count = " << anemometer_copy << endl;
		loop_count = 0;
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	gpio.output(GPIO::pd2, 0);
	gpio.input(GPIO::pd3, 1);

	EICRA = _BV(ISC11);
	EIMSK = _BV(INT1);

	arch.idle_loop();

	return 0;
}

ISR(INT1_vect) {
	anemometer_count++;
}
