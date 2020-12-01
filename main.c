#include <stdio.h>
#include <wm8731.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "I2C_FreeRtos.h"
#include "task.h"
#include "semphr.h"
#include "fsl_uart.h"
#include "filtros.h"

/* TODO: insert other definitions and declarations here. */
/*
 * @brief   Application entry point.
 */

#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(SYS_CLK)
#define DEMO_UART          UART0

SemaphoreHandle_t i2c_sem;
SemaphoreHandle_t uart_sem;

uint32_t Buffer[4 * 1024];
uint32_t rxBuffer = 0;
void init_uart(void);

void init_codec(void *parameters)
{
	uint8_t g_codec_sucess = freertos_i2c_fail;
	g_codec_sucess = wm8731_init();
	if(freertos_i2c_sucess == g_codec_sucess)
	{
		PRINTF("Codec configured\n\r");
	}
	wm8731_i2s_config();
	//wm8731_Activate();
	Data_Buffer(&Buffer[0]);
	init_uart();
	xSemaphoreGive(i2c_sem);
	vTaskSuspend(NULL);
}

void codec_audio(void *parameters)
{

	xSemaphoreTake(i2c_sem, portMAX_DELAY);

	for(;;)
	{
		/*TODO AUDIO FUNCTIONS*/
		wm8731_getData((uint8_t*) (Buffer+rxBuffer), 1024);
		rxBuffer++;
			if(rxBuffer>= 4)
			{
				rxBuffer = 0U;
				xSemaphoreGive(uart_sem);

			}
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}

uint8_t *filter_data = 0;
void uart_inst(void *parameters)
{

	xSemaphoreTake(uart_sem, portMAX_DELAY);

	for(;;)
	{
		/* Tomar datos de la uart*/
		UART_ReadBlocking(DEMO_UART, filter_data, 3);
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}

void init_uart(void)
{
	static uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx     = true;
	config.enableRx     = true;

	UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

	uint8_t txbuff[]   = "Codec audio practice 4\r\nChoose your filter\r\n\r\nLow pass (LP)\n\rHigh pass (HP)\n\rBand Pass (BP)\r\n";
	UART_WriteBlocking(DEMO_UART, txbuff, sizeof(txbuff) - 1);

	/* Tomar datos de la uart*/
	uint8_t i = 0;
	uint8_t ch;
	do{
		UART_ReadBlocking(DEMO_UART, &ch, 1);
		UART_WriteBlocking(DEMO_UART,&ch, 1);
		*(filter_data+i) = ch;
		i++;
	} while(i < 3);

}

int main(void)
{
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif


    i2c_sem = xSemaphoreCreateBinary();
    uart_sem = xSemaphoreCreateBinary();

    xTaskCreate(init_codec, "init_codec", 110, NULL, 1, NULL);
    xTaskCreate(codec_audio, "codec_audio", 110, NULL, 1, NULL);
   // xTaskCreate(uart_inst, "uart", 110, NULL, 1, NULL);

    vTaskStartScheduler();

    while(1)
    {

    }
    return 0 ;
}
