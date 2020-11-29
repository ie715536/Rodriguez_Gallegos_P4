/*
 * I2C.h
 *
 *  Created on: 14 oct. 2020
 *      Author: urik_
 */

#ifndef I2C_FREERTOS_H_
#define I2C_FREERTOS_H_

#include "stdint.h"
#include "fsl_i2c.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "FreeRTOS.h"
#include "semphr.h"

/**Defines number of serial ports*/
#define NUMBER_OF_SERIAL_PORTS (4)

typedef enum
{
	freertos_i2c0,
	freertos_i2c1,
	freertos_i2c2,
	freertos_i2c3
} freertos_i2c_number_t;

typedef enum
{
	freertos_i2c_portA,
	freertos_i2c_portB,
	freertos_i2c_portC,
	freertos_i2c_portD,
	freertos_i2c_portE
} freertos_i2c_port_t;

typedef enum
{
	freertos_i2c_sucess,
	freertos_i2c_fail
} freertos_i2c_flag_t;

typedef struct
{
	uint32_t baudrate;
	freertos_i2c_number_t i2c_number;
	freertos_i2c_port_t port;
	uint8_t SCL;
	uint8_t SDA;
	uint8_t pin_mux;
}freertos_i2c_config_t ;

freertos_i2c_flag_t freertos_i2c_init(freertos_i2c_config_t config);
freertos_i2c_flag_t freertos_i2c_transfer(freertos_i2c_number_t i2c_number, uint8_t * buffer, uint16_t length, uint8_t slaveAddress, uint32_t subaddress, uint8_t subaddressSize);
freertos_i2c_flag_t freertos_i2c_receive(freertos_i2c_number_t i2c_number, uint8_t * buffer, uint16_t lenght, uint8_t slaveAddress, uint32_t subaddress, uint8_t subaddressSize);

#endif /* I2C_FREERTOS_H_ */
