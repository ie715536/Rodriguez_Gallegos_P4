/**
 * @file    freertos_uart.c
 * @brief   Application entry point.
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


/* TODO: insert other definitions and declarations here. */

void init_WM8731_task(void * args);
void Audio_task(void * args);

SemaphoreHandle_t WM8731_sem;

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
	xSemaphoreGive(WM8731_sem);
	vTaskDelay(portMAX_DELAY); /** Delay infinito... para suspenderlo usar vTaskSuspend(NULL); */
}


void Audio_task(void * args)
{
	xSemaphoreTake(WM8731_sem, portMAX_DELAY);

	for(;;)
	{
		vTaskDelay(pdMS_TO_TICKS(400));
	}
}


