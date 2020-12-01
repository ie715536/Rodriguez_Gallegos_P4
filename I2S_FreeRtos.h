/*
 * I2S_FreeRtos.h
 *
 *  Created on: 29 nov. 2020
 *      Author: urik_
 */

#ifndef I2S_FREERTOS_H_
#define I2S_FREERTOS_H_

#include "fsl_sai.h"
#include "fsl_sai_edma.h"

#define DEMO_CODEC_DA7212
#define DEMO_SAI_CHANNEL      (0)
#define DEMO_SAI_CLKSRC       kCLOCK_CoreSysClk
#define DEMO_SAI_CLK_FREQ     CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_SAI_IRQ          I2S0_Tx_IRQn
#define DEMO_SAITxIRQHandler  I2S0_Tx_IRQHandler
#define DEMO_SAI_TX_SYNC_MODE kSAI_ModeAsync
#define DEMO_SAI_RX_SYNC_MODE kSAI_ModeSync
#define DEMO_SAI_MCLK_OUTPUT  true
#define DEMO_SAI_MASTER_SLAVE kSAI_Master

#define DEMO_DMA           DMA0
#define DEMO_EDMA_CHANNEL  (0)
#define DEMO_SAI_TX_SOURCE kDmaRequestMux0I2S0Tx

#define DEMO_AUDIO_DATA_CHANNEL (2U)
#define DEMO_AUDIO_BIT_WIDTH    kSAI_WordWidth16bits
#define DEMO_AUDIO_SAMPLE_RATE  (kSAI_SampleRate16KHz)
#define DEMO_AUDIO_MASTER_CLOCK 12288000U

void I2S_init();
void I2S_stopTransfer();

#endif /* I2S_FREERTOS_H_ */
