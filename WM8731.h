/*
 * WM8731.h
 *
 *  Created on: 13 nov. 2020
 *      Author: urik_
 */

#ifndef WM8731_H_
#define WM8731_H_

#include <stdio.h>
#include "I2C_FreeRtos.h"
#include "fsl_clock.h"
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define BAUDRATE 100000U
#define I2C0_SCL_PIN 2
#define I2C0_SDA_PIN 3

/** Max Delay of I2C in ms */
#define I2C_DELAY 400
/** Data length */
#define I2C_INIT_DATA_LENGTH 1U
/** Sub Address Size */
#define I2C_SUB_ADD_SIZE 1U



#define WM8731_SLAVE_ADDRESS		0x1A

#define WM8731_LEFT_LINE_IN			0x00 //
#define WM8731_RIGHT_LINE_IN		0x01 //
#define WM8731_LEFT_HEADPHONE_OUT	0x02
#define WM8731_RIGHT_HEADPHONE_OUT  0x03
#define WM8731_ANALOGUE_AUDIO		0x04 //
#define WM8731_INTERFACE_FORMAT     0x07
#define WM8731_SAMPLING_CONTROL     0x08
#define WM8731_POWER_DOWN_REGISTER	0x0C //
#define WM8731_RESET_REGISTER	    0x0F //
#define WM8731_CODEC_ENABLE			0x12

#define Low(param) ((char *)&param)[0]
#define Hig(param) ((char *)&param)[1]

typedef enum
{
	freertos_WM8731_sucess,
	freertos_WM8731_fail
} freertos_WM8731_flag_t;

freertos_WM8731_flag_t WM8731_init();

#endif /* WM8731_H_ */
