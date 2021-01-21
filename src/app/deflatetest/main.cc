/*
 * Copyright 2020 Daniel Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#include "driver/counter.h"

#include "lib/inflate.h"


/*
// bad apple 0042.png
unsigned char const deflate_input[] = {
	40, 207, 99, 96, 64, 0, 102, 134, 81, 14, 117, 57, 236, 84, 229, 240, 147, 193, 145, 167, 148, 99, 79, 41, 7, 183, 209, 243, 113, 114, 80, 148, 161, 248, 7, 37, 120, 71, 16, 0, 0, 108, 36, 4, 56
};
*/

/*
// bad apple 1000.png
unsigned char const deflate_input[] = {
	40, 207, 173, 210, 189, 13, 194, 48, 16, 5, 224, 103, 82, 208, 225, 17, 204, 32, 72, 153, 129, 13, 50, 4, 189, 205, 36, 180, 244, 44, 16, 38, 193, 27, 96, 58, 138, 196, 6, 147, 31, 191, 68, 73, 4, 18, 215, 125, 186, 211, 249, 78, 62, 4, 10, 252, 11, 55, 198, 137, 97, 52, 1, 146, 145, 37, 120, 136, 132, 10, 72, 120, 2, 186, 135, 3, 114, 134, 226, 50, 201, 153, 4, 124, 7, 55, 130, 154, 125, 71, 241, 160, 146, 86, 192, 154, 51, 3, 40, 46, 235, 32, 34, 218, 125, 28, 100, 213, 163, 4, 118, 61, 12, 98, 52, 168, 64, 112, 27, 194, 195, 19, 66, 3, 253, 27, 68, 124, 170, 131, 140, 107, 119, 80, 193, 166, 214, 58, 162, 93, 193, 7, 202, 52, 131, 15, 160, 167, 97, 150, 144, 243, 255, 228, 211, 101, 131, 110, 118, 116, 33, 51, 153, 241, 4, 233, 196, 182, 4, 95, 112, 198, 50, 74, 190, 81, 187, 128, 108, 2, 245, 37, 220, 223, 168, 63, 184, 30, 87, 197, 254, 112, 22, 75, 221, 94, 223, 233, 137, 202
};
*/


unsigned char const deflate_input[] = {
	120, 156, 243, 72, 205, 201, 201, 215, 81, 8, 207, 47, 202, 73, 177, 87, 240, 64, 226, 41, 2, 0, 128, 125, 9, 17
};


/*
unsigned char const deflate_input[] = {
	120, 1, 243, 72, 204, 201, 201, 215, 81, 8, 79, 205, 41, 81, 4, 0, 26, 155, 3, 250
};
*/

unsigned char deflate_output[1024];

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	for (uint8_t i = 0; i < 5; i++) {
		counter.start();
		int16_t ret = inflate_zlib((unsigned char*)deflate_input, sizeof(deflate_input), deflate_output, sizeof(deflate_output));
		counter.stop();
		kout << "inflate returned " << ret << endl;
		kout << "Output: " << (char*)deflate_output << endl;
		kout << "took " << counter.value << "/" << counter.overflow << " cycles" << endl;
	}

	arch.idle();

	return 0;
}
