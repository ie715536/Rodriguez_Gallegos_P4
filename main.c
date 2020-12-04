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
#include "I2S_FreeRtos.h"
#include "filtros.h"

/* TODO: insert other definitions and declarations here. */
/*
 * @brief   Application entry point.
 */


#define D_UART_CLK CLOCK_GetFreq(SYS_CLK)
#define D_UART UART0


SemaphoreHandle_t initialization_sem;
uint32_t Buffer[4*1024];
uint32_t rxBuffer = 0;

const char* LPE = "LPE";
const char* HPE = "HPE";
const char* BPE = "BPE";
const char* LPD = "LPD";
const char* HPD = "HPD";
const char* BPD = "BPD";

static char uart_data[3];

void init_codec(void *parameters)
{
	uint8_t g_codec_sucess  = freertos_i2c_fail;
	g_codec_sucess  = wm8731_init();
	if(freertos_i2c_sucess != g_codec_sucess)
	{
		PRINTF("I2C INICIALIZADO INCORRECTAMETNE\n\r");
	}

	//CONFIGURAR I2S

	while(1)
	{
		xSemaphoreGive(initialization_sem);

		vTaskDelay(portMAX_DELAY);
	}

}

void init_uart(void *parameters)
{
	Data_Buffer(Buffer);
	static uart_config_t configuration;
	UART_GetDefaultConfig(&configuration);
	configuration.baudRate_Bps = 115200;
	configuration.enableTx     = true;
	configuration.enableRx     = true;

	UART_Init(D_UART, &configuration, D_UART_CLK);

	uint8_t buffer[]   = "CODEC AUDIO FILTER MENU \r\n FILTRO:\r\n\r\n LOW PASS ENABLE\t[LPE]\n\r HIGH PASS ENABLE\t[HPE]\n\r BAND PASS ENABLE\t[BPE]\r\n";
	UART_WriteBlocking(D_UART, buffer, sizeof(buffer) - 1);

	while(1)
	{
		uint8_t i = 0;
		uint8_t ch;
		do{
			UART_ReadBlocking(D_UART, &ch, 1);
			UART_WriteBlocking(D_UART,&ch, 1);
			uart_data[i] = ch;
			i++;
		} while(i < 3);


		if(strcmp(LPE,uart_data) == 0)
		{
			Call_Filter(LP);
		}
		else if(strcmp(HPE,uart_data) == 0)
		{
			Call_Filter(HP);
		}
		else if(strcmp(BPE,uart_data) == 0)
		{
			Call_Filter(BP);
		}
		else if(strcmp(LPD,uart_data) == 0 || strcmp(HPD,uart_data) == 0 || strcmp(BPD,uart_data) == 0)
		{
			Call_Filter(BYPASS);
		}
		UART_WriteBlocking(D_UART,"\r   \r", 5);
	}

}

void codec_audio(void *parameters)
{
	xSemaphoreTake(initialization_sem, portMAX_DELAY);

	while(1)
	{
		/*
		codec_rx((uint8_t*)(Buffer+rxBuffer),1024);
		rxBuffer++;

		if(rxBuffer >= 4)
		{
			rxBuffer = 0U;
		}
		*/
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

    initialization_sem = xSemaphoreCreateBinary();

    xTaskCreate(init_uart, "initialize UART", 110, NULL, 1, NULL);
    xTaskCreate(init_codec, "init project", 110, NULL, 1, NULL);
    xTaskCreate(codec_audio, "get audio", 110, NULL, 1, NULL);


    vTaskStartScheduler();

    for(;;)
    {

    }
    return 0 ;
}

