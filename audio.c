/*
 * audio.c
 *
 *  Created on: 30 nov. 2020
 *      Author: urik_
 */


#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
#include "FreeRTOSConfig.h"
/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
#include "I2C_FreeRtos.h"
#include "WM8731.h"
#include "semphr.h"


extern sai_handle_t sai_rx_handle;

freertos_i2c_config_t i2c_config;

uint8_t Buffer[4*1024];

uint32_t rxBuffer = 0;

static const port_pin_config_t i2c_pin_config =
{
		kPORT_PullUp,
		kPORT_SlowSlewRate,
		kPORT_PassiveFilterDisable,
		kPORT_OpenDrainEnable,
		kPORT_LowDriveStrength,
		kPORT_MuxAlt5,
		kPORT_UnlockRegister,
};



static void audio_config (void *arg);

static void audio_get_data (void *arg);

void audio_init (void)
{
	i2c_config.baudrate = BAUDRATE;
	i2c_config.i2c_number = freertos_i2c0;
	i2c_config.port = freertos_i2c_portE;
	i2c_config.SCL =  I2C0_SCL_2_PIN;
	i2c_config.SDA = I2C0_SDA_2_PIN;
	i2c_config.pin_mux = kPORT_MuxAlt5;

	audio_handle.init_end = xSemaphoreCreateBinary();

	xTaskCreate(audio_config, "audio_config", 4*configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(audio_get_data, "audio_get_data", 4*configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();


}



















