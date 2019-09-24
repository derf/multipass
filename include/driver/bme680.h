/**
 * Copyright (C) 2017 - 2018 Bosch Sensortec GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 * @file	bme680.h
 * @date	19 Jun 2018
 * @version	3.5.9
 * @brief
 *
 * Adjustments for multipass by Daniel Friesel: C -> C++, multipass i2c integration
 *
 */
/*! @file bme680.h
 @brief Sensor driver for BME680 sensor */
/*!
 * @defgroup BME680 SENSOR API
 * @{*/
#ifndef BME680_H_
#define BME680_H_

/* Header includes */
#include "driver/bme680_defs.h"

class BME680 {
	private:
		BME680(const BME680 &copy);
		unsigned char txbuf[16];
		unsigned char rxbuf[8];

		/*! Chip Id */
		uint8_t chip_id;
		/*! Device Id */
		uint8_t dev_id;
		/*! Memory page used */
		uint8_t mem_page;
		/*! Sensor calibration data */
		struct bme680_calib_data calib;
		/*! New sensor fields */
		uint8_t new_fields;
		/*! Store the info messages */
		uint8_t info_msg;
		/*! Communication function result */
		int8_t com_rslt;

		/*!
		* @brief This internal API is used to read the calibrated data from the sensor.
		*
		* This function is used to retrieve the calibration
		* data from the image registers of the sensor.
		*
		* @note Registers 89h  to A1h for calibration data 1 to 24
		*        from bit 0 to 7
		* @note Registers E1h to F0h for calibration data 25 to 40
		*        from bit 0 to 7
		*
		* @return Result of API execution status.
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getCalibData();

		/*!
		* @brief This internal API is used to set the gas configuration of the sensor.
		*
		* @return Result of API execution status.
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t setGasConfig();

		/*!
		* @brief This internal API is used to get the gas configuration of the sensor.
		* @note heatr_temp and heatr_dur values are currently register data
		* and not the actual values set
		*
		* @return Result of API execution status.
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getGasConfig();

		/*!
		* @brief This internal API is used to calculate the Heat duration value.
		*
		* @param[in] dur	:Value of the duration to be shared.
		*
		* @return uint8_t threshold duration after calculation.
		*/
		uint8_t calcHeaterDur(uint16_t dur);

#ifndef BME680_FLOAT_POINT_COMPENSATION

		/*!
		* @brief This internal API is used to calculate the temperature value.
		*
		* @param[in] temp_adc	:Contains the temperature ADC value .
		*
		* @return uint32_t calculated temperature.
		*/
		int16_t calcTemperature(uint32_t temp_adc);

		/*!
		* @brief This internal API is used to calculate the pressure value.
		*
		* @param[in] pres_adc	:Contains the pressure ADC value .
		*
		* @return uint32_t calculated pressure.
		*/
		uint32_t calcPressure(uint32_t pres_adc);

		/*!
		* @brief This internal API is used to calculate the humidity value.
		*
		* @param[in] hum_adc	:Contains the humidity ADC value.
		*
		* @return uint32_t calculated humidity.
		*/
		uint32_t calcHumidity(uint16_t hum_adc);

		/*!
		* @brief This internal API is used to calculate the Gas Resistance value.
		*
		* @param[in] gas_res_adc	:Contains the Gas Resistance ADC value.
		* @param[in] gas_range		:Contains the range of gas values.
		*
		* @return uint32_t calculated gas resistance.
		*/
		uint32_t calcGasResistance(uint16_t gas_res_adc, uint8_t gas_range);

		/*!
		* @brief This internal API is used to calculate the Heat Resistance value.
		*
		* @param[in] temp	: Contains the target temperature value.
		*
		* @return uint8_t calculated heater resistance.
		*/
		uint8_t calcHeaterRes(uint16_t temp);

#else
		/*!
		* @brief This internal API is used to calculate the
		* temperature value value in float format
		*
		* @param[in] temp_adc	:Contains the temperature ADC value .
		*
		* @return Calculated temperature in float
		*/
		float calcTemperature(uint32_t temp_adc);

		/*!
		* @brief This internal API is used to calculate the
		* pressure value value in float format
		*
		* @param[in] pres_adc	:Contains the pressure ADC value .
		*
		* @return Calculated pressure in float.
		*/
		float calcPressure(uint32_t pres_adc);

		/*!
		* @brief This internal API is used to calculate the
		* humidity value value in float format
		*
		* @param[in] hum_adc	:Contains the humidity ADC value.
		*
		* @return Calculated humidity in float.
		*/
		float calcHumidity(uint16_t hum_adc);

		/*!
		* @brief This internal API is used to calculate the
		* gas resistance value value in float format
		*
		* @param[in] gas_res_adc	:Contains the Gas Resistance ADC value.
		* @param[in] gas_range		:Contains the range of gas values.
		*
		* @return Calculated gas resistance in float.
		*/
		float calcGasResistance(uint16_t gas_res_adc, uint8_t gas_range);

		/*!
		* @brief This internal API is used to calculate the
		* heater resistance value in float format
		*
		* @param[in] temp	: Contains the target temperature value.
		*
		* @return Calculated heater resistance in float.
		*/
		float calcHeaterRes(uint16_t temp);

#endif

		/*!
		* @brief This internal API is used to calculate the field data of sensor.
		*
		* @param[out] data :Structure instance to hold the data
		*
		*  @return int8_t result of the field data from sensor.
		*/
		int8_t readFieldData(struct bme680_field_data *data);

		/*!
		* @brief This internal API is used to set the memory page
		* based on register address.
		*
		* The value of memory page
		*  value  | Description
		* --------|--------------
		*   0     | BME680_PAGE0_SPI
		*   1     | BME680_PAGE1_SPI
		*
		* @param[in] reg_addr	:Contains the register address array.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t setMemPage(uint8_t reg_addr);

		/*!
		* @brief This internal API is used to get the memory page based
		* on register address.
		*
		* The value of memory page
		*  value  | Description
		* --------|--------------
		*   0     | BME680_PAGE0_SPI
		*   1     | BME680_PAGE1_SPI
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getMemPage();

		/*!
		* @brief This internal API is used to validate the device pointer for
		* null conditions.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t nullPtrCheck();

		/*!
		* @brief This internal API is used to check the boundary
		* conditions.
		*
		* @param[in] value	:pointer to the value.
		* @param[in] min	:minimum value.
		* @param[in] max	:maximum value.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t boundaryCheck(uint8_t *value, uint8_t min, uint8_t max);

	public:

		/*! SPI/I2C interface */
		enum bme680_intf intf;
		/*! Sensor power modes */
		uint8_t power_mode;
		/*! Sensor settings */
		struct bme680_tph_sett tph_sett;
		/*! Gas Sensor settings */
		struct bme680_gas_sett gas_sett;
		/*! Ambient temperature in Degree C */
		int8_t amb_temp;
		/*! Bus read function pointer */
		bme680_com_fptr_t read;
		/*! Bus write function pointer */
		bme680_com_fptr_t write;
		/*! delay function pointer */
		bme680_delay_fptr_t delay_ms;

		BME680(uint8_t const addr) : dev_id(addr) {}

		/*!
		*  @brief This API is the entry point.
		*  It reads the chip-id and calibration data from the sensor.
		*
		*  @return Result of API execution status
		*  @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t init();

		/*!
		* @brief This API writes the given data to the register address
		* of the sensor.
		*
		* @param[in] reg_addr : Register address from where the data to be written.
		* @param[in] reg_data : Pointer to data buffer which is to be written
		* in the sensor.
		* @param[in] len : No of bytes of data to write..
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t setRegs(const uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len);

		/*!
		* @brief This API reads the data from the given register address of the sensor.
		*
		* @param[in] reg_addr : Register address from where the data to be read
		* @param[out] reg_data : Pointer to data buffer to store the read data.
		* @param[in] len : No of bytes of data to be read.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getRegs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

		/*!
		* @brief This API performs the soft reset of the sensor.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
		*/
		int8_t softReset();

		/*!
		* @brief This API is used to set the power mode of the sensor.
		*
		* @note : Pass the value to bme680_dev.power_mode structure variable.
		*
		*  value	|	mode
		* -------------|------------------
		*	0x00	|	BME680_SLEEP_MODE
		*	0x01	|	BME680_FORCED_MODE
		*
		* * @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t setSensorMode();

		/*!
		* @brief This API is used to get the power mode of the sensor.
		*
		* @note : bme680_dev.power_mode structure variable hold the power mode.
		*
		*  value	|	mode
		* ---------|------------------
		*	0x00	|	BME680_SLEEP_MODE
		*	0x01	|	BME680_FORCED_MODE
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getSensorMode();

		/*!
		* @brief This API is used to set the profile duration of the sensor.
		*
		* @param[in] duration : Duration of the measurement in ms.
		*
		* @return Nothing
		*/
		void setProfileDur(uint16_t duration);

		/*!
		* @brief This API is used to get the profile duration of the sensor.
		*
		* @param[in] duration : Duration of the measurement in ms.
		*
		* @return Nothing
		*/
		void getProfileDur(uint16_t *duration);

		/*!
		* @brief This API reads the pressure, temperature and humidity and gas data
		* from the sensor, compensates the data and store it in the bme680_data
		* structure instance passed by the user.
		*
		* @param[out] data: Structure instance to hold the data.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error
		*/
		int8_t getSensorData(struct bme680_field_data *data);

		/*!
		* @brief This API is used to set the oversampling, filter and T,P,H, gas selection
		* settings in the sensor.
		*
		* @param[in] desired_settings : Variable used to select the settings which
		* are to be set in the sensor.
		*
		*	 Macros	                   |  Functionality
		*---------------------------------|----------------------------------------------
		*	BME680_OST_SEL             |    To set temperature oversampling.
		*	BME680_OSP_SEL             |    To set pressure oversampling.
		*	BME680_OSH_SEL             |    To set humidity oversampling.
		*	BME680_GAS_MEAS_SEL        |    To set gas measurement setting.
		*	BME680_FILTER_SEL          |    To set filter setting.
		*	BME680_HCNTRL_SEL          |    To set humidity control setting.
		*	BME680_RUN_GAS_SEL         |    To set run gas setting.
		*	BME680_NBCONV_SEL          |    To set NB conversion setting.
		*	BME680_GAS_SENSOR_SEL      |    To set all gas sensor related settings
		*
		* @note : Below are the macros to be used by the user for selecting the
		* desired settings. User can do OR operation of these macros for configuring
		* multiple settings.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
		*/
		int8_t setSensorSettings(uint16_t desired_settings);

		/*!
		* @brief This API is used to get the oversampling, filter and T,P,H, gas selection
		* settings in the sensor.
		*
		* @param[in] desired_settings : Variable used to select the settings which
		* are to be get from the sensor.
		*
		* @return Result of API execution status
		* @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
		*/
		int8_t getSensorSettings(uint16_t desired_settings);
};

extern BME680 bme680;

#endif /* BME680_H_ */
/** @}*/
