#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"

void loop(void)
{
	static unsigned char done = 0;

	if (!done) {
		kout << "sizeof(char) = " << sizeof(char) << endl;
		kout << "sizeof(short) = " << sizeof(short) << endl;
		kout << "sizeof(int) = " << sizeof(int) << endl;
		kout << "sizeof(long int) = " << sizeof(long int) << endl;
		kout << "sizeof(long long) = " << sizeof(long long) << endl;
		kout << "sizeof(float) = " << sizeof(float) << endl;
		kout << "sizeof(double) = " << sizeof(double) << endl;
		done = 1;
	}
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "Hello, World!" << endl;

	arch.idle_loop();

	return 0;
}
