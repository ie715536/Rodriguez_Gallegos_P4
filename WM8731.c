/*
 * WM8731.c
 *
 *  Created on: 13 nov. 2020
 *      Author: urik_
 */

#include "WM8731.h"
#define I2C_SCL		24U
#define I2C_SDA		25U
#define BAUD_RATE	100000
#define I2C_DELAY	100//(ms)


/* SAI and I2C instance and clock */
#define DEMO_SAI              I2S0
#define DEMO_SAI_CHANNEL      (0)
#define DEMO_SAI_CLKSRC       kCLOCK_CoreSysClk
#define DEMO_SAI_CLK_FREQ     CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_SAI_IRQ          I2S0_Tx_IRQn
#define DEMO_SAITxIRQHandler  I2S0_Tx_IRQHandler
#define DEMO_SAI_TX_SYNC_MODE kSAI_ModeAsync
#define DEMO_SAI_RX_SYNC_MODE kSAI_ModeSync
#define DEMO_SAI_MCLK_OUTPUT  true
#define DEMO_SAI_MASTER_SLAVE kSAI_Master

#define DEMO_DMA             DMA0
#define DEMO_EDMA_CHANNEL    (0)
#define DEMO_DMAMUX          DMAMUX
#define DEMO_TX_EDMA_CHANNEL (0U)
#define DEMO_RX_EDMA_CHANNEL (1U)
#define DEMO_SAI_TX_SOURCE   kDmaRequestMux0I2S0Tx
#define DEMO_SAI_RX_SOURCE   kDmaRequestMux0I2S0Rx

/*Global declaratios*/
static freertos_i2c_config_t WM8731_i2c_config;
freertos_i2c_flag_t wm8731_sucess = freertos_i2c_fail;

freertos_i2c_flag_t wm8731_init(void)
{
	/*Start port and freertos i2c inicialization */
	WM8731_i2c_config.baudrate = BAUD_RATE;
	WM8731_i2c_config.i2c_number = 	freertos_i2c0;
	WM8731_i2c_config.port = freertos_i2c_portE;
	WM8731_i2c_config.SCL = I2C_SCL;
	WM8731_i2c_config.SDA = I2C_SDA;
	WM8731_i2c_config.pin_mux = kPORT_MuxAlt5;

	/*Init configuration for freertos i2c*/
	wm8731_sucess = freertos_i2c_init(WM8731_i2c_config);
	vTaskDelay(pdMS_TO_TICKS(10));

	if(freertos_i2c_sucess == wm8731_sucess)
	{
		wm8731_sucess = freertos_i2c_fail;

		/*Initial configuration of bmi160*/
		uint8_t data[2] = {WM8731_REG_RESET, _WM8731_RESET};
		/*Acc config*/
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_LLINE_IN;
		data[1] = _WM8731_LEFT_LINEIN;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_RLINE_IN;
		data[1] = _WM8731_RIGHT_LINEIN;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));


		data[0] = WM8731_REG_LHPHONE_OUT;
		data[1] = _WM8731_LEFT_HP;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_RHPHONE_OUT;
		data[1] = _WM8731_RIGHT_HP;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_ANALOG_PATH;
		data[1] = _WM8731_ANALOGAUDIO;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_DIGITAL_PATH;
		data[1] = _WM8731_DIGITALAUDIO;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_PDOWN_CTRL;
		data[1] = _WM8731_POWER;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_DIGITAL_IF;
		data[1] = _WM8731_DAIF;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

		data[0] = WM8731_REG_SAMPLING_CTRL;
		data[1] = _WM8731_SAMPLING;
		wm8731_sucess = freertos_i2c_transfer(WM8731_i2c_config.i2c_number, data , 2, WM8731_SLAVE_ADDRESS);
		vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));


		wm8731_Activate();

	}
	return wm8731_sucess;
}

uint8_t aux_data[2] = {0};

void wm8731_play_audio(void)
{

	/* Deactive module*/
	aux_data[0] = WM8731_REG_ACTIVE_CTRL;
	aux_data[1] = _WM8731_DEACTIVATE;
	freertos_i2c_transfer(WM8731_i2c_config.i2c_number, aux_data , 2, WM8731_SLAVE_ADDRESS);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));
}


void wm8731_Activate(void)
{
	/* Activate module*/

	aux_data[0] = WM8731_REG_ACTIVE_CTRL;
	aux_data[1] = _WM8731_ACTIVATE;
	freertos_i2c_transfer(WM8731_i2c_config.i2c_number, aux_data , 2, WM8731_SLAVE_ADDRESS);
	vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));

}
sai_handle_t sai_tx_handle;
sai_handle_t sai_rx_handle;
static  sai_transceiver_t config;



static void txCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
static void rxCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);


edma_config_t dmaConfig = {0};
sai_edma_handle_t txHandle = {0};
sai_edma_handle_t rxHandle = {0};
edma_handle_t dmaRxHandle;
edma_handle_t dmaTxHandle;

sai_master_clock_t mclkConfig;


void wm8731_i2s_config(void)
{

//	/* Init DMAMUX */
//	DMAMUX_Init(DEMO_DMAMUX);
//	DMAMUX_SetSource(DEMO_DMAMUX, DEMO_TX_EDMA_CHANNEL, (uint8_t)DEMO_SAI_TX_SOURCE);
//	DMAMUX_EnableChannel(DEMO_DMAMUX, DEMO_TX_EDMA_CHANNEL);
//	DMAMUX_SetSource(DEMO_DMAMUX, DEMO_RX_EDMA_CHANNEL, (uint8_t)DEMO_SAI_RX_SOURCE);
//	DMAMUX_EnableChannel(DEMO_DMAMUX, DEMO_RX_EDMA_CHANNEL);
//
//
//	/* Init DMA and create handle for DMA */
//	EDMA_GetDefaultConfig(&dmaConfig);
//	EDMA_Init(DEMO_DMA, &dmaConfig);
//	EDMA_CreateHandle(&dmaTxHandle, DEMO_DMA, DEMO_TX_EDMA_CHANNEL);
//	EDMA_CreateHandle(&dmaRxHandle, DEMO_DMA, DEMO_RX_EDMA_CHANNEL);

	/* SAI init */
	SAI_Init(DEMO_SAI);

	SAI_TransferTxCreateHandleEDMA(DEMO_SAI, &txHandle, txCallback, NULL, &dmaTxHandle);
	SAI_TransferRxCreateHandleEDMA(DEMO_SAI, &rxHandle, rxCallback, NULL, &dmaRxHandle);

	/* I2S mode configurations */
	SAI_GetClassicI2SConfig(&config, DEMO_AUDIO_BIT_WIDTH, kSAI_Stereo, 1U << DEMO_SAI_CHANNEL);
	config.syncMode    = DEMO_SAI_TX_SYNC_MODE;
	config.masterSlave = kSAI_Slave;
	SAI_TransferTxSetConfigEDMA(DEMO_SAI, &txHandle, &config);
	config.syncMode = DEMO_SAI_RX_SYNC_MODE;
	SAI_TransferRxSetConfigEDMA(DEMO_SAI, &rxHandle, &config);

//	SAI_Init(I2S0);
//	SAI_TransferRxCreateHandle(I2S0, &sai_rx_handle, rx_callback, NULL);
//	SAI_GetClassicI2SConfig(&config, kSAI_WordWidth24bits, kSAI_Stereo, 1<<0);
//
//	config.bitClock.bclkSource = kSAI_BclkSourceBusclk;
//	config.syncMode = kSAI_ModeAsync;
//	config.masterSlave = kSAI_Slave;
//	config.frameSync.frameSyncPolarity = kSAI_SampleOnRisingEdge;
//
//	SAI_TransferRxSetConfig(I2S0, &sai_rx_handle,&config);
//
//	vTaskDelay(pdMS_TO_TICKS(100));
//
//	SAI_RxEnableInterrupts(I2S0, kSAI_FIFOErrorInterruptEnable);
//	EnableIRQ(I2S0_Rx_IRQn);

	/*Init I2S*/
	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);
	/* Port E Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortE);

	/* PORTC1 (pin B11) is configured as I2S0_TXD0 data select */
	PORT_SetPinMux(PORTC, 1U, kPORT_MuxAlt6);

	/* PORTC1 (pin B11) is configured as I2S0_MCLK*/
	PORT_SetPinMux(PORTC, 6U, kPORT_MuxAlt6);

	/* PORTE11 (pin G4) is configured as I2S0_TX_FS Word select*/
	PORT_SetPinMux(PORTE, 11U, kPORT_MuxAlt4);

	/* PORTE12 (pin G3) is configured as I2S0_TX_BCLK This is the bit clock */
	PORT_SetPinMux(PORTE, 12U, kPORT_MuxAlt4);

	/* PORTE7 (pin F4) is configured as I2S0_RXD0 */
	PORT_SetPinMux(PORTC, 5U, kPORT_MuxAlt4);

	/* set bit clock divider */
	SAI_TxSetBitClockRate(DEMO_SAI, DEMO_AUDIO_MASTER_CLOCK, DEMO_AUDIO_SAMPLE_RATE, DEMO_AUDIO_BIT_WIDTH,
			DEMO_AUDIO_DATA_CHANNEL);
	SAI_RxSetBitClockRate(DEMO_SAI, DEMO_AUDIO_MASTER_CLOCK, DEMO_AUDIO_SAMPLE_RATE, DEMO_AUDIO_BIT_WIDTH,
			DEMO_AUDIO_DATA_CHANNEL);

	/* master clock configurations */
	mclkConfig.mclkOutputEnable = true, mclkConfig.mclkHz = DEMO_AUDIO_MASTER_CLOCK;
	mclkConfig.mclkSourceClkHz = DEMO_SAI_CLK_FREQ;
	SAI_SetMasterClockConfig(DEMO_SAI, &mclkConfig);;

	SAI_RxEnableInterrupts(I2S0, kSAI_FIFOErrorInterruptEnable);
	EnableIRQ(I2S0_Rx_IRQn);

}


void wm8731_getData(uint8_t* data, size_t length)
{
	/*RX FUNCTION*/
	sai_transfer_t xfer;
	xfer.data =data;
	xfer.dataSize = length;

	if(kStatus_Success == SAI_TransferReceiveNonBlocking(I2S0, &sai_rx_handle, &xfer))
	{

	}

}

static void txCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}



