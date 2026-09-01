/*
 * Copyright 2026 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include "arch.h"
#include "config.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#include "driver/counter.h"

#include "model.h"
#define MODEL_NAME serialized_model

/*
 * Required for tfmicro / assert()
 */
extern "C" {
volatile void _exit() {}
};
#include <cstdio>
FILE *const stderr = nullptr;

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

constexpr int kTensorArenaSize = CONFIG_app_tfmicro_test_arena_size * 1024;
uint8_t tensor_arena[kTensorArenaSize];

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	kout << "TFmicro test" << endl;

	model = tflite::GetModel(MODEL_NAME);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		kout << "Unsupported schema version" << endl;
		while (1);
	}

	kout << "Model loaded" << endl;
	static tflite::AllOpsResolver resolver;
	static tflite::MicroInterpreter static_interpreter(
		model, resolver, tensor_arena, kTensorArenaSize);
	interpreter = &static_interpreter;

	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) {
		kout << "AllocateTensors() failed: " << allocate_status << endl;
		while (1);
	}

	while (1) {
		counter.start();
		TfLiteStatus invoke_status = interpreter->Invoke();
		counter.stop();
		if (invoke_status != kTfLiteOk) {
			kout << "invoke failed: " << invoke_status << endl;
			while (1);
		}
		kout << "interpreter->Invoke() = " << counter.value << "/" << counter.overflow << " cycles" << endl;
		gpio.led_toggle(0);
	}

	arch.idle();

	return 0;
}
