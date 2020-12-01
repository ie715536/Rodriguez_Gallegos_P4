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


void init_WM8731_task(void * args);
void Audio_task(void * args);

SemaphoreHandle_t WM8731_sem;
SemaphoreHandle_t UART_sem;

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

void init_uart (void)
{
	static uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = true;
	config.enableRx = true;
	UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

	uint8_t txbuff[] = "codec";
	UART_WriteBlocking(DEMO_UART, txbuff, sizeof(txbuff) -1);

	for(uint8_t i = 0; i < 3; i++)
	{
		UART_ReadBlocking(DEMO_UART, &filter_data[i], 1);
		UART_WriteBlocking(DEMO_UART, &filter_data[i], 1);
	}
}

int main(void)
{

  /* Init board hardware. */
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
  /* Init FSL debug console. */
  BOARD_InitDebugConsole();

  xTaskCreate(init_WM8731_task, "init_WM8731", 110, NULL, 1, NULL);
  xTaskCreate(Audio_task, "audio_data", 110, NULL, 1, NULL);

  WM8731_sem = xSemaphoreCreateBinary();

  vTaskStartScheduler();

  volatile static int i = 0 ;

  while(1)
  {
    i++;
    __asm volatile ("nop");
  }

  return 0 ;
}

void init_WM8731_task(void * args)
{
	freertos_WM8731_flag_t status = freertos_WM8731_fail;
	status = WM8731_init();
	if(freertos_WM8731_sucess != status)
	{
		PRINTF("FAIL INITIALIZATION");
		for(;;);
	}
	WM8731_I2S_config();
	WM8731_activate();
	xSemaphoreGive(WM8731_sem);
	vTaskDelay(portMAX_DELAY); /** Delay infinito... para suspenderlo usar vTaskSuspend(NULL); */
}


void Audio_task(void * args)
{
	xSemaphoreTake(WM8731_sem, portMAX_DELAY);

	for(;;)
	{
		WM8731_GetData((unit8_t*) (Buffer+rxBuffer), 1024);
		rxBuffer++;

		if(rxBuffer >= 4)
		{
			rxBuffer = 0;
			xSemaphoreGive(UART_sem);
		}
		vTaskDelay(pdMS_TO_TICKS(400));
	}
}







