/*
 * Copyright 2022 Birte Kristina Friesel
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "config.h"
#ifdef CONFIG_driver_bme680_bsec_save_state
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define BSEC_STATE_PATH TOSTRING(CONFIG_driver_bme680_bsec_state_path)
#include <stdio.h>
#endif

#include "arch.h"
#include "driver/gpio.h"
#include "driver/stdout.h"
#include "driver/uptime.h"
#if defined(CONFIG_meta_driver_hardware_i2c)
#include "driver/i2c.h"
#else
#include "driver/soft_i2c.h"
#endif
#include "driver/bme680.h"
#include "driver/bme680_util.h"
#include "driver/bme680-bsec-armv6/bsec_interface.h"
#include "driver/max44009.h"

bsec_bme_settings_t sensor_settings;

void load_bsec_state()
{
	uint8_t serialized_state[BSEC_MAX_STATE_BLOB_SIZE];
	uint8_t work_buffer[BSEC_MAX_STATE_BLOB_SIZE];
	FILE *f = fopen(BSEC_STATE_PATH, "r");
	if (f != NULL) {
		size_t serialized_state_size = fread(serialized_state, BSEC_MAX_STATE_BLOB_SIZE, sizeof(uint8_t), f);
		if (serialized_state_size > 0) {
			bsec_library_return_t bsec_status = bsec_set_state(serialized_state, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_STATE_BLOB_SIZE);
			if (bsec_status < 0) {
				kout << "# bsec_set_state error: " << bsec_status << endl;
			}
			if (bsec_status > 0) {
				kout << "# bsec_set_state warning: " << bsec_status << endl;
			}
		}
		if (fclose(f) == EOF) {
			perror("fclose");
		}
	} else {
		// file doesn't exist. that's harmless.
		perror("fopen");
	}
}

void save_bsec_state()
{
	uint32_t  serialized_state_size;
	uint8_t serialized_state[BSEC_MAX_STATE_BLOB_SIZE];
	uint8_t work_buffer[BSEC_MAX_STATE_BLOB_SIZE];

	bsec_library_return_t status = bsec_get_state(0, serialized_state, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_STATE_BLOB_SIZE, &serialized_state_size);
	if (status < 0) {
		kout << "# bsec_get_state error: " << status << endl;
		return;
	}
	if (status > 0) {
		kout << "# bsec_get_state warning: " << status << endl;
	}
	FILE *f = fopen(BSEC_STATE_PATH, "w");
	if (f == NULL) {
		perror("fopen");
		return;
	}
	if (fwrite(serialized_state, sizeof(uint8_t), serialized_state_size, f) < serialized_state_size) {
		perror("fwrite");
	}
	if (fclose(f) == EOF) {
		perror("fclose");
	}
}

int configure_bsec()
{
	bsec_sensor_configuration_t virtual_sensors[8];
	unsigned char n_virtual_sensors = 8;

	bsec_sensor_configuration_t sensor_configs[BSEC_MAX_PHYSICAL_SENSOR];
	unsigned char n_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

	float sample_rate = BSEC_SAMPLE_RATE_LP;

	virtual_sensors[0].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE;
	virtual_sensors[0].sample_rate = sample_rate;
	virtual_sensors[1].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY;
	virtual_sensors[1].sample_rate = sample_rate;
	virtual_sensors[2].sensor_id = BSEC_OUTPUT_RAW_PRESSURE;
	virtual_sensors[2].sample_rate = sample_rate;
	virtual_sensors[3].sensor_id = BSEC_OUTPUT_RAW_GAS;
	virtual_sensors[3].sample_rate = sample_rate;
	virtual_sensors[4].sensor_id = BSEC_OUTPUT_IAQ;
	virtual_sensors[4].sample_rate = sample_rate;
	virtual_sensors[5].sensor_id = BSEC_OUTPUT_RAW_TEMPERATURE;
	virtual_sensors[5].sample_rate = sample_rate;
	virtual_sensors[6].sensor_id = BSEC_OUTPUT_RAW_HUMIDITY;
	virtual_sensors[6].sample_rate = sample_rate;
	virtual_sensors[7].sensor_id = BSEC_OUTPUT_STATIC_IAQ;
	virtual_sensors[7].sample_rate = sample_rate;

	bsec_library_return_t bsec_status = bsec_update_subscription(virtual_sensors, n_virtual_sensors, sensor_configs, &n_sensor_settings);

	if (bsec_status != BSEC_OK) {
		kout << "# bsec_update_subscription error: " << bsec_status << endl;
		return 1;
	}
	kout << "# bsec_update_subscription OK" << endl;
	return 0;
}

void control_bsec(int64_t now)
{
	struct bme680_field_data data;
	bsec_input_t bsec_inputs[BSEC_MAX_PHYSICAL_SENSOR];
	bsec_output_t bsec_outputs[BSEC_NUMBER_OUTPUTS];

	uint8_t num_bsec_inputs = 0;
	uint8_t num_bsec_outputs = BSEC_NUMBER_OUTPUTS;

	bsec_library_return_t status = bsec_sensor_control(now, &sensor_settings);

	if (status < 0) {
		kout << "# bsec_sensor_control error: " << status << endl;
		return;
	}
	if (status > 0) {
		kout << "# bsec_sensor_control warning: " << status << endl;
	}

	if (!sensor_settings.trigger_measurement) {
		return;
	}

	bme680.tph_sett.os_hum = sensor_settings.humidity_oversampling;
	bme680.tph_sett.os_pres = sensor_settings.pressure_oversampling;
	bme680.tph_sett.os_temp = sensor_settings.temperature_oversampling;
	bme680.gas_sett.run_gas = sensor_settings.run_gas;
	bme680.gas_sett.heatr_temp = sensor_settings.heater_temperature;
	bme680.gas_sett.heatr_dur = sensor_settings.heating_duration;

	bme680.power_mode = BME680_FORCED_MODE;
	bme680.setSensorSettings(BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL | BME680_GAS_SENSOR_SEL);

	bme680.setSensorMode();


	/*
	 * TODO recent versions of the bme680 open-source driver are able to
	 * calculate the required delay.
	 */
	arch.delay_ms(250);

	do {
		arch.delay_ms(5);
		bme680.getSensorMode();
	} while (bme680.power_mode == BME680_FORCED_MODE);

	if (sensor_settings.process_data) {
		bme680.getSensorData(&data);
		if (data.status & BME680_NEW_DATA_MSK) {
			if (sensor_settings.process_data & BSEC_PROCESS_TEMPERATURE) {
				bsec_inputs[num_bsec_inputs].sensor_id = BSEC_INPUT_TEMPERATURE;
				bsec_inputs[num_bsec_inputs].signal = data.temperature / 100.0f;
				bsec_inputs[num_bsec_inputs].time_stamp = now;
				num_bsec_inputs++;
			}
			if (sensor_settings.process_data & BSEC_PROCESS_HUMIDITY) {
				bsec_inputs[num_bsec_inputs].sensor_id = BSEC_INPUT_HUMIDITY;
				bsec_inputs[num_bsec_inputs].signal = data.humidity / 1000.0f;
				bsec_inputs[num_bsec_inputs].time_stamp = now;
				num_bsec_inputs++;
			}
			if (sensor_settings.process_data & BSEC_PROCESS_PRESSURE) {
				bsec_inputs[num_bsec_inputs].sensor_id = BSEC_INPUT_PRESSURE;
				bsec_inputs[num_bsec_inputs].signal = data.pressure;
				bsec_inputs[num_bsec_inputs].time_stamp = now;
				num_bsec_inputs++;
			}
			if (sensor_settings.process_data & BSEC_PROCESS_GAS) {
				bsec_inputs[num_bsec_inputs].sensor_id = BSEC_INPUT_GASRESISTOR;
				bsec_inputs[num_bsec_inputs].signal = data.gas_resistance;
				bsec_inputs[num_bsec_inputs].time_stamp = now;
				num_bsec_inputs++;
			}
		}
	}

	if (num_bsec_inputs > 0) {
		status = bsec_do_steps(bsec_inputs, num_bsec_inputs, bsec_outputs, &num_bsec_outputs);

		if (status < 0) {
			kout << "# bsec_do_steps error: " << status << endl;
			return;
		}
		if (status > 0) {
			kout << "# bsec_do_steps warning: " << status << endl;
		}

		kout << "bme680 ";
		for (uint8_t i = 0; i < num_bsec_outputs; i++) {
			if (i > 0) {
				kout << ",";
			}
			switch (bsec_outputs[i].sensor_id) {
				case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
					kout << "temperature_celsius=" << bsec_outputs[i].signal;
					break;
				case BSEC_OUTPUT_RAW_TEMPERATURE:
					kout << "raw_temperature_celsius=" << bsec_outputs[i].signal;
					break;
				case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
					kout << "humidity_relpercent=" << bsec_outputs[i].signal;
					break;
				case BSEC_OUTPUT_RAW_HUMIDITY:
					kout << "raw_humidity_relpercent=" << bsec_outputs[i].signal;
					break;
				case BSEC_OUTPUT_RAW_PRESSURE:
					kout << "pressure_hpa=" << bsec_outputs[i].signal / 100;
					break;
				case BSEC_OUTPUT_RAW_GAS:
					kout << "air_quality_ohm=" << bsec_outputs[i].signal;
					break;
				case BSEC_OUTPUT_IAQ:
					if (bsec_outputs[i].accuracy > 0) {
						kout << "air_quality_index=" << bsec_outputs[i].signal << ",";
					}
					kout << "air_quality_accuracy_index=" << bsec_outputs[i].accuracy;
					break;
				case BSEC_OUTPUT_STATIC_IAQ:
					kout << "air_quality_raw=" << bsec_outputs[i].signal;
					break;
				default:
					continue;
			}
		}
		kout << endl;
	}
}

void loop(void)
{
	static uint16_t i = 0;
	int64_t now = uptime.get_us() * 1000;

	if ((now < sensor_settings.next_call) && (sensor_settings.next_call - now < 1000000000)) {
		// less than one second -> sleep
		arch.delay_us((sensor_settings.next_call - now) / 1000);
		now = uptime.get_us() * 1000;
	}

	if (now >= sensor_settings.next_call) {
		control_bsec(now);
	}

	if ((i%20) == 0) {
		float lux = max44009.getLux();
		if (lux >= 0) {
			kout << "max44009 illuminance_lux=" << max44009.getLux() << endl;
		} else {
			kout << "# MAX44009 error" << endl;
		}
	}

	if ((i%1800) == 0) {
		save_bsec_state();
	}

	i++;
}

int main(void)
{
	arch.setup();
	gpio.setup();
	kout.setup();

	while (i2c.setup() != 0) {
		kout << "# I2C setup failed" << endl;
		return 1;
	}
	kout << "# I2C setup OK" << endl;

	bme680.intf = BME680_I2C_INTF;
	bme680.read = bme680_i2c_read;
	bme680.write = bme680_i2c_write;
	bme680.delay_ms = bme680_delay_ms;

	int8_t bme680_status = bme680.init();
	while (bme680_status != 0) {
		kout << "# BME680 init failed: " << (uint8_t)bme680_status << endl;
		return 1;
	}
	kout << "# BME680 init OK" << endl;

	bsec_library_return_t bsec_status = bsec_init();
	while (bsec_status != BSEC_OK) {
		kout << "# BSEC init failed: " << bsec_status << endl;
		return 1;
	}
	kout << "# BSEC init OK" << endl;

	load_bsec_state();

	if (configure_bsec() != 0) {
		return 1;
	}

	arch.idle_loop();

	return 0;
}
