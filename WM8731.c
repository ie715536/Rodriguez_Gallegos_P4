/*
 * WM8731.c
 *
 *  Created on: 13 nov. 2020
 *      Author: urik_
 */

#include "WM8731.h"

static freertos_i2c_config_t i2c_config;

freertos_WM8731_flag_t WM8731_init()
{
	freertos_WM8731_flag_t status = freertos_WM8731_fail;

	/** Enables Clock I2C0 in Port B */
	CLOCK_EnableClock(kCLOCK_PortB);

	i2c_config.baudrate = BAUDRATE;
	i2c_config.i2c_number = freertos_i2c0;
	i2c_config.port = freertos_i2c_portB;
	i2c_config.SCL =  I2C0_SCL_PIN;
	i2c_config.SDA = I2C0_SDA_PIN;
	i2c_config.pin_mux = kPORT_MuxAlt2;

	status = freertos_i2c_init(i2c_config);

	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY)); /** Transform Max delay into Ticks and waits*/

	if(freertos_WM8731_sucess != status)
	{
		return status;
	}

	status = freertos_WM8731_fail;
	uint8_t regis;
	uint16_t value;
	uint8_t add;
	uint8_t buffer;

	/** Power down register */
	regis = WM8731_POWER_DOWN_REGISTER;
	buffer = 0x00;

	vTaskDelay(pdMS_TO_TICKS(10));

	add = regis << 1;
	add = add | (Hig(value)&1);
	buffer = Low(value);

	status = freertos_i2c_transfer(i2c_config.i2c_number, &buffer , I2C_INIT_DATA_LENGTH, WM8731_SLAVE_ADDRESS, add, I2C_SUB_ADD_SIZE);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

	/** Left line in  */
	regis = WM8731_LEFT_LINE_IN;
	buffer = 23;

	vTaskDelay(pdMS_TO_TICKS(10));

	add = regis << 1;
	add = add | (Hig(value)&1);
	buffer = Low(value);

	status = freertos_i2c_transfer(i2c_config.i2c_number, &buffer , I2C_INIT_DATA_LENGTH, WM8731_SLAVE_ADDRESS, add, I2C_SUB_ADD_SIZE);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

	/** Right line in */
	regis = WM8731_RIGHT_LINE_IN;
	buffer = 23;

	vTaskDelay(pdMS_TO_TICKS(10));

	add = regis << 1;
	add = add | (Hig(value)&1);
	buffer = Low(value);

	status = freertos_i2c_transfer(i2c_config.i2c_number, &buffer , I2C_INIT_DATA_LENGTH, WM8731_SLAVE_ADDRESS, add, I2C_SUB_ADD_SIZE);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));


	/** Analogue Audio */
	regis = WM8731_ANALOGUE_AUDIO;
	buffer = 0x05;

	vTaskDelay(pdMS_TO_TICKS(10));

	add = regis << 1;
	add = add | (Hig(value)&1);
	buffer = Low(value);

	status = freertos_i2c_transfer(i2c_config.i2c_number, &buffer , I2C_INIT_DATA_LENGTH, WM8731_SLAVE_ADDRESS, add, I2C_SUB_ADD_SIZE);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

	/** Codec Enable */
	regis = WM8731_CODEC_ENABLE;
	buffer = 0x01;

	vTaskDelay(pdMS_TO_TICKS(10));

	add = regis << 1;
	add = add | (Hig(value)&1);
	buffer = Low(value);

	status = freertos_i2c_transfer(i2c_config.i2c_number, &buffer , I2C_INIT_DATA_LENGTH, WM8731_SLAVE_ADDRESS, add, I2C_SUB_ADD_SIZE);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

	return status;
}

