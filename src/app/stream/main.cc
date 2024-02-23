/*
 * Copyright 2020 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/counter.h"
#include <stdlib.h>

#define XSTR(x) STR(x)
#define STR(x) #x

static const auto input_size = CONFIG_app_stream_n_elements;
static const auto stride = CONFIG_app_stream_stride;
typedef CONFIG_app_stream_type T;
static T A[input_size];
static T B[input_size];
static T scale;
static T X[input_size];
static T check;

int main(void)
{
	uint16_t i;
	arch.setup();
	gpio.setup();
	kout.setup();

	scale = rand();
	for (i = 0; i < input_size; i++) {
		A[i] = (T)rand();
		B[i] = (T)rand();
		X[i] = (T)0;
	}

	while (1) {
		check = 0;
		/*
		 * Copy
		 */
		counter.start();
		for (i = 0; i < input_size; i += stride) {
			X[i] = A[i];
		}
		counter.stop();

		kout << "[::] STREAM COPY";
		kout << " | n_elements=" << input_size << " e_type=" << XSTR(CONFIG_app_stream_type) << " elem_B=" << sizeof(T) << " n_stride=" << stride;
		kout << " | latency_us=" << counter.value << "/" << counter.overflow;
		kout << endl;

		for (i = 0; i < input_size; i++) {
			check = (uint16_t)check ^ (uint16_t)X[i];
		}

		/*
		 * Scale
		 */
		counter.start();
		for (i = 0; i < input_size; i += stride) {
			X[i] = scale * A[i];
		}
		counter.stop();

		kout << "[::] STREAM SCALE";
		kout << " | n_elements=" << input_size << " e_type=" << XSTR(CONFIG_app_stream_type) << " elem_B=" << sizeof(T) << " n_stride=" << stride;
		kout << " | latency_us=" << counter.value << "/" << counter.overflow;
		kout << endl;

		for (i = 0; i < input_size; i++) {
			check = (uint16_t)check ^ (uint16_t)X[i];
		}

		/*
		 * Add
		 */
		counter.start();
		for (i = 0; i < input_size; i += stride) {
			X[i] = A[i] + B[i];
		}
		counter.stop();

		kout << "[::] STREAM ADD";
		kout << " | n_elements=" << input_size << " e_type=" << XSTR(CONFIG_app_stream_type) << " elem_B=" << sizeof(T) << " n_stride=" << stride;
		kout << " | latency_us=" << counter.value << "/" << counter.overflow;
		kout << endl;

		for (i = 0; i < input_size; i++) {
			check = (uint16_t)check ^ (uint16_t)X[i];
		}

		/*
		 * Triad
		 */
		counter.start();
		for (i = 0; i < input_size; i += stride) {
			X[i] = A[i] + scale * B[i];
		}
		counter.stop();

		kout << "[::] STREAM TRIAD";
		kout << " | n_elements=" << input_size << " e_type=" << XSTR(CONFIG_app_stream_type) << " elem_B=" << sizeof(T) << " n_stride=" << stride;
		kout << " | latency_us=" << counter.value << "/" << counter.overflow;
		kout << endl;

		for (i = 0; i < input_size; i++) {
			check = (uint16_t)check ^ (uint16_t)X[i];
		}

		/*
		 * Avoid optimizations
		 */
		kout << "// " << check << endl;
	}

	return 0;
}
